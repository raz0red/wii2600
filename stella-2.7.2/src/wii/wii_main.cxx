/*
Wii2600 : Port of Stella for the Wii

Copyright (C) 2009
raz0red (www.twitchasylum.com)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/


#include <wiiuse/wpad.h>
#include <gccore.h>
#include <stdio.h>
#include <sdl.h>

//#include "atari.h"

//#include "../util.h"

#include "wii_main.hxx"
#include "wii_config.hxx"
#include "wii_file_io.hxx"
#include "wii_image.hxx"
#include "wii_hw_buttons.hxx"
#include "wii_app.hxx"
#include "wii_stelladaptor.hxx"
#include "wii_input.hxx"

#include <sys/dir.h>

#define ABOUT_Y 20
#define MENU_HEAD_X 3
#define MENU_HEAD_Y 9
#define MENU_FOOT_Y 27
#define MENU_START_Y 12
#define MENU_START_X 8
#define MENU_PAGE_SIZE 13

// The screen scale
int wii_scale = 1;
// The vsync mode
int wii_vsync = 0;
// Gamma adjust
int wii_gamma = 50;
// Force FPS
int wii_force_fps = -1;
// Whether this is the first time we have run
BOOL wii_first_run = 1;
// Whether to display rom info 
BOOL wii_display_rom_info = FALSE;

// The average FPS from testing vsync 
static float test_fps = 0.0;
// Whether the Wii is PAL or NTSC
static BOOL is_pal = 0;

// Whether to display debug info (FPS, etc.)
short wii_debug = 0;
// The stack containing the menus the user has navigated
TREENODEPTR wii_menu_stack[4];
// The head of the menu stack
s8 wii_menu_stack_head = -1;
// Two framebuffers (double buffer our menu display)
u32 *wii_xfb[2] = { NULL, NULL };
 
// Buffer containing the about image
static u32* about_buff = NULL;
// Properties of the about image
static PNGUPROP about_props;
// The root of the menu
static TREENODE *menu_root;
// The current frame buffer
static u32 cur_xfb = 0;
// Are we still running?
static BOOL quit = 0;
// Is the menu to be redrawn?
static BOOL redraw_menu = 1;
// The current menu selection
static s16 menu_cur_idx = -1;
// The first item to display in the menu (paging, etc.)
static s16 menu_start_idx = 0;

// The main args
static int main_argc;
static char **main_argv;

// Forward refs 
static void wii_free_node( TREENODE* node );

// Stella's main
extern int stellaMain(int argc, char* argv[]);

// Gamma adjust
extern "C" {
extern void WII_UpdateGamma( int gamma );
}

/*
 * Test to see if the machine is PAL or NTSC
 */
static void wii_test_pal()
{
    // TODO: Can we simply use the current TV mode to determine this?
    int start = SDL_GetTicks();
    for( int i = 0; i < 40; i++ )
    {
        VIDEO_WaitVSync();
    }
    int time = SDL_GetTicks() - start;

    test_fps = time / 40.0;

    is_pal = test_fps > 17.5;
}

/*
 * Sets whether to enable VSYNC or not 
 *
 * param    sync Whether to enable VSYNC or not
 */
static void wii_set_vsync( BOOL sync )
{
    if( sync )
    {
        wii_vsync = VSYNC_ENABLED;
        if( is_pal )
        {
            wii_force_fps = 50;
        }
    }
    else
    {
        wii_vsync = VSYNC_DISABLED;
        wii_force_fps = -1;
    }
}

/*
 * Resets the menu indexes when an underlying menu in change (push/pop)
 */
void wii_menu_reset_indexes()
{
    menu_cur_idx = -1;
    menu_start_idx = 0;
}

/*
 * Creates and returns a new menu tree node
 *
 * type     The type of the node
 * name     The name for the node
 * return   The newly created node
 */
static TREENODE* wii_create_tree_node( enum NODETYPE type, const char *name )
{
    TREENODE* nodep = (TREENODE*)malloc( sizeof( TREENODE ) );
    memset( nodep, 0, sizeof( TREENODE ) );
    nodep->node_type = type;
    nodep->name = strdup( name );
    nodep->child_count = 0;
    nodep->max_children = 0;

    return nodep;
}

/*
 * Adds the specified child node to the specified parent
 *
 * parent   The parent
 * child    The child to add to the parent
 */
static void wii_add_child( TREENODE *parent, TREENODE *childp  )
{
    // Do we have room?
    if( parent->child_count == parent->max_children )
    {
        parent->max_children += 10;
        parent->children = (TREENODEPTR*)
            realloc( parent->children, parent->max_children * 
            sizeof(TREENODEPTR));		
    }
    
    parent->children[parent->child_count++] = childp;
}

/*
 * Clears the children for the specified node
 *
 * node     The node to clear the children for
 */
static void wii_clear_children( TREENODE* node )
{
    int i;
    for( i = 0; i < node->child_count; i++ )
    {
        wii_free_node( node->children[i] );
        node->children[i] = NULL;
    }
    node->child_count = 0;
}

/*
 * Frees the specified tree node
 *
 * node     The node to free
 */
static void wii_free_node( TREENODE* node )
{
    wii_clear_children( node );    

    free( node->children );
    free( node->name );	
    free( node );
}

/*
 * Pushes the specified menu onto the menu stack (occurs when the user
 * navigates to a sub-menu)
 *
 * menu     The menu to push on the stack
 */
static void wii_menu_push( TREENODE *menu )
{    
    wii_menu_stack[++wii_menu_stack_head] = menu;

    wii_menu_reset_indexes();
    wii_menu_move( menu, 1 );
}

/*
 * Pops the latest menu off of the menu stack (occurs when the user leaves 
 * the current menu.
 *
 * return   The pop'd menu
 */
TREENODE* wii_menu_pop()
{    
    if( wii_menu_stack_head > 0 )
    {
        TREENODE *oldmenu = wii_menu_stack[wii_menu_stack_head--];
        wii_menu_reset_indexes();        
        wii_menu_move( wii_menu_stack[wii_menu_stack_head], 1 );
        return oldmenu;
    }

    return NULL;
}

/*
 * Updates the buffer with the header message for the current menu
 *
 * menu     The menu
 * buffer   The buffer to update with the header message for the
 *          current menu.
 */
static void wii_menu_get_header( TREENODE* menu, char *buffer )
{
    switch( menu->node_type )
    {
#if 0
        case NODETYPE_LOAD_ROM:
            if( !games_read )
            {
                snprintf( buffer, WII_MENU_BUFF_SIZE, "Reading game list..." );                
            }
            break;
#endif
    }

    if( strlen( buffer ) == 0 )
    {
        snprintf( buffer, WII_MENU_BUFF_SIZE, 
            "U/D = Scroll%s, A = Select%s, Home = Exit", 
            ( menu->child_count > MENU_PAGE_SIZE ? ", L/R = Page" : "" ),
            ( wii_menu_stack_head > 0 ? ", B = Back" : "" )            
        );
    }
}

/*
 * Updates the buffer with the footer message for the current menu
 *
 * menu     The menu
 * buffer   The buffer to update with the footer message for the
 *          current menu.
 */
static void wii_menu_get_footer( TREENODE* menu, char *buffer )
{    
    // Any status messages? If so display it in the footer
    if( wii_status_message_count > 0 )
    {        
        wii_status_message_count--;
        snprintf( buffer, WII_MENU_BUFF_SIZE, wii_status_message );
    }
    else
    {
        switch( menu->node_type )
        {
#if 0
            case NODETYPE_LOAD_ROM:
                if( games_read )
                {
                    wii_get_list_footer( menu, "cartridge", buffer );
                }
                break;
#endif
        }
    }
}

/*
 * Updates the buffer with the name of the specified node
 *
 * node     The node
 * buffer   The name of the specified node
 */
static void wii_menu_get_node_name( TREENODE* node, char *buffer )
{
    BOOL enabled = FALSE;
    char *strmode = NULL;

    switch( node->node_type )
    {
        case NODETYPE_SCALE:    
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %dx", node->name, 
                wii_scale + 1 );            
            break;
        case NODETYPE_GAMMA:    
            if( wii_gamma == -1 )
            {
                snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: Disabled", 
                    node->name );
            }
            else
            {
                snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %d%s", node->name, 
                    wii_gamma, ( wii_gamma == 50 ? " (default)" : "" ) );            
            }
            break;
        case NODETYPE_PADDLE_ROLL_CENTER:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s%d%s", node->name, 
                ( wii_paddle_roll_center > 0 ? "+" : "" ),
                wii_paddle_roll_center, 
                ( wii_paddle_roll_center == WIIMOTE_ROLL_CENTER_ADJ_VALUE_DEFAULT ? 
                    " (default)" : "" ) );                        
            break;
        case NODETYPE_PADDLE_ROLL_SENSITIVITY:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %d%s", node->name, 
                wii_paddle_roll_sensitivity, 
                ( wii_paddle_roll_sensitivity == WIIMOTE_ROLL_SENSITIVITY_DEFAULT ? 
                    " (default)" : "" ) );                        
            break;
        case NODETYPE_PADDLE_IR_SENSITIVITY:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %d%s", node->name, 
                wii_paddle_ir_sensitivity, 
                ( wii_paddle_ir_sensitivity == WIIMOTE_IR_SENSITIVITY_DEFAULT ? 
                    " (default)" : "" ) );                        
            break;
        case NODETYPE_PADDLE_ANALOG_SENSITIVITY:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %d%s", node->name, 
                wii_paddle_analog_sensitivity, 
                ( wii_paddle_analog_sensitivity == PADDLE_ANALOG_SENSITIVITY_DEFAULT ? 
                    " (default)" : "" ) );                        
            break;
        case NODETYPE_PADDLE_ANALOG_AXIS:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s", node->name, 
                ( wii_paddle_analog_xaxis ? "X (horizontal)" : "Y (vertical)" ) );
            break;
        case NODETYPE_DEBUG_MODE:          
            switch( node->node_type )
            {
                case NODETYPE_DEBUG_MODE:
                    enabled = wii_debug;
                    break;
            }
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s", node->name, 
                enabled ? "Enabled" : "Disabled" );
            break;
        case NODETYPE_VSYNC:
            switch( wii_vsync )
            {
                case VSYNC_DISABLED:
                    strmode="Disabled";
                    break;
                case VSYNC_ENABLED:
                    strmode="Enabled";
                    break;
#if 0
                case VSYNC_DOUBLE_BUFF:
                    strmode="Enabled + double buffer";
                    break;
#endif
            }
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s", node->name, strmode );
            break;
        case NODETYPE_DISPLAY_ROM_INFO:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s", node->name, 
                wii_display_rom_info ? "Enabled" : "Disabled" );
            break;
        case NODETYPE_PADDLE_MODE:
            switch( wii_paddle_mode )
            {
                case WII_PADDLE_MODE_ANALOG:
                    strmode="Analog (Joystick)";
                    break;
                case WII_PADDLE_MODE_MOTE_IR:
                    strmode="IR (Wiimote)";
                    break;
                case WII_PADDLE_MODE_MOTE_ROLL:
                    strmode="Roll (Wiimote)";
                    break;
            }
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s", node->name, strmode );
            break;
        case NODETYPE_FORCE_FPS:
            switch( wii_force_fps )
            {
                case -1:
                    strmode="Disabled";
                    break;
                case 50:
                    strmode="50 (PAL)";
                    break;
                case 60:
                    strmode="60 (NTSC)";
                    break;
            }
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %s", node->name, strmode );
            break;
        case NODETYPE_TEST_FPS:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s: %f (%s)", node->name, 
                test_fps, ( is_pal ? "PAL" : "NTSC" ) );
            break;
        default:	
            snprintf( buffer, WII_MENU_BUFF_SIZE, node->name );
    }
}

/*
 * React to the "select" event for the specified node
 *
 * node     The node
 */
static void wii_menu_select_node( TREENODE *node )
{
    switch( node->node_type )
    {
        case NODETYPE_START_EMULATION:
        {
            // Write out the current config
            wii_write_config();

            // Use the SDL frame buffer for emulation
            u32 *fb = wii_xfb[0];  

            // Bind the debug console based on the "debug" setting
            wii_console_init( wii_debug ? fb : wii_xfb[1] );

            VIDEO_ClearFrameBuffer( display_mode, wii_xfb[0], COLOR_BLACK );
            VIDEO_ClearFrameBuffer( display_mode, wii_xfb[1], COLOR_BLACK );			
            VIDEO_SetNextFramebuffer( fb );
            VIDEO_Flush();
            VIDEO_WaitVSync();		

#if 0
fprintf( stderr, "\n\n" );
fprintf( stderr, "found stelladaptor: %d", wii_stelladaptor_open() );
wii_pause();
#endif
            WII_UpdateGamma( wii_gamma );

            stellaMain( main_argc, main_argv );

            // Exit
            quit = TRUE;

            break;
        }
        case NODETYPE_SCALE:
        {
            wii_scale++;
            if( wii_scale > 1 ) 
            {
                wii_scale = 0;
            }			
            break;
        }
        case NODETYPE_DISPLAY_ROM_INFO:
        {
            wii_display_rom_info = !wii_display_rom_info;
            break;
        }
        case NODETYPE_PADDLE_ANALOG_AXIS:
        {
            wii_paddle_analog_xaxis = !wii_paddle_analog_xaxis;
            break;
        }
        case NODETYPE_PADDLE_MODE:
        {
            wii_paddle_mode++;
            if( wii_paddle_mode > WII_PADDLE_MODE_ANALOG ) 
            {
                wii_paddle_mode = 0;
            }			
            break;
        }
        case NODETYPE_PADDLE_ROLL_SENSITIVITY:
        {
            wii_paddle_roll_sensitivity++;
            if( wii_paddle_roll_sensitivity > WIIMOTE_ROLL_SENSITIVITY_LEVELS ) 
            {
                wii_paddle_roll_sensitivity = 1;
            }			
            break;
        }
        case NODETYPE_PADDLE_ROLL_CENTER:
        {
            wii_paddle_roll_center++;
            if( wii_paddle_roll_center > 5 ) 
            {
                wii_paddle_roll_center = -5;
            }			
            break;
        }
        case NODETYPE_PADDLE_IR_SENSITIVITY:
        {
            wii_paddle_ir_sensitivity++;
            if( wii_paddle_ir_sensitivity > WIIMOTE_IR_SENSITIVITY_LEVELS ) 
            {
                wii_paddle_ir_sensitivity = 1;
            }			
            break;
        }
        case NODETYPE_PADDLE_ANALOG_SENSITIVITY:
        {
            wii_paddle_analog_sensitivity++;
            if( wii_paddle_analog_sensitivity > PADDLE_ANALOG_SENSITIVITY_LEVELS ) 
            {
                wii_paddle_analog_sensitivity = 1;
            }			
            break;
        }
        case NODETYPE_DEBUG_MODE:
        {
            wii_debug ^= 1;
            break;
        }
        case NODETYPE_VSYNC:
        {            
            wii_set_vsync( wii_vsync ^ 1 );
            break;
        }
        case NODETYPE_GAMMA:
        {            
            if( wii_gamma == -1 )
            {
                wii_gamma = 5;
            }
            else
            {
                wii_gamma += 5;
                if( wii_gamma > 100 )
                {
                    wii_gamma = -1;
                }
            }
            break;
        }
        case NODETYPE_ADVANCED:
        case NODETYPE_CONTROLS:
        {
            wii_menu_push( node );
            break;
        }                   
        case NODETYPE_FORCE_FPS:
        {
            if( wii_force_fps == -1 )
            {
                wii_force_fps = 50;
            }
            else if( wii_force_fps == 50 )
            {
                wii_force_fps = 60;
            }
            else
            {
                wii_force_fps = -1;
            }
            break;
        }
        case NODETYPE_TEST_FPS:
        {
            wii_test_pal();
        }
        break;
    }
}

/*
 * Determines whether the node is currently visible
 *
 * node     The node
 * return   Whether the node is visible
 */
static BOOL wii_menu_is_node_visible( TREENODE *node )
{
    switch( node->node_type )
    {
#if 0
        case NODETYPE_SAVE_SNAPSHOT:
        case NODETYPE_RESET:
        case NODETYPE_RESUME:
            return wii_last_rom != NULL;
            break;
#endif
    }

    return TRUE;
}

/*
 * Determines whether the node is selectable
 *
 * node     The node
 * return   Whether the node is selectable
 */
static BOOL wii_menu_is_node_selectable( TREENODE *node )
{
    if( !wii_menu_is_node_visible( node ) )
    {
        return FALSE;
    }

    switch( node->node_type )
    {
        case NODETYPE_SPACER:
            return FALSE;
            break;
    }

    return TRUE;
}

/*
 * Provides an opportunity for the specified menu to do something during 
 * a display cycle.
 *
 * menu     The menu
 */
static void wii_menu_update( TREENODE *menu )
{
    switch( menu->node_type )
    {
#if 0
        case NODETYPE_LOAD_ROM:
            if( !games_read )
            {
                wii_read_game_list( menu );  
                wii_menu_reset_indexes();    
                wii_menu_move( menu, 1 );
                redraw_menu = 1;
            }
            break;
#endif
    }
}

/*
 * Renders the specified menu
 */
static void wii_menu_render( TREENODE *menu )
{	
    // Swap buffers
    cur_xfb ^= 1;
    u32 *fb = wii_xfb[cur_xfb];

    VIDEO_ClearFrameBuffer( display_mode, fb, COLOR_BLACK );

    // Bind the console to the appropriate frame buffer
    wii_console_init( fb );

    // Render the about image (header)
    if( about_buff != NULL )
    {
        wii_draw_image( 
            &about_props, 
            about_buff, 
            fb, 
            ( display_mode->fbWidth - about_props.imgWidth ) >> 1,  
            ABOUT_Y );
    }

    // Draw the menu items (text)
    if( menu )
    {	
        char buffer[WII_MENU_BUFF_SIZE];
        char buffer2[WII_MENU_BUFF_SIZE];

        buffer[0] = '\0';
        wii_menu_get_header( menu, buffer );

        snprintf( buffer2, sizeof(buffer2), "\x1b[%d;%dH %s", 
            MENU_HEAD_Y, MENU_HEAD_X, buffer );		

        wii_write_vt( buffer2 );

        int displayed = 0; 
        int i;
        for( i = menu_start_idx; 
            i < menu->child_count && displayed < MENU_PAGE_SIZE; 
            i++ )
        {		
            buffer[0] = '\0';

            TREENODE* node = menu->children[i];
            if( wii_menu_is_node_visible( node ) )
            {
                wii_menu_get_node_name( node, buffer );

                snprintf( buffer2, sizeof(buffer2), "\x1b[%d;%dH%s %s %s", 				
                    MENU_START_Y + displayed, 
                    MENU_START_X, 
                    ( menu_cur_idx == i ? "\x1b[41m\x1b[37m" : "" ),
                    buffer,
                    ( menu_cur_idx == i ? "\x1b[40m\x1b[37m" : "" )              
                );

                wii_write_vt( buffer2 );
                ++displayed;
            }
        }

        buffer[0] = '\0';
        wii_menu_get_footer( menu, buffer );

        snprintf( buffer2, sizeof(buffer2), "\x1b[%d;%dH %s", 
            MENU_FOOT_Y, MENU_HEAD_X, buffer );		

        wii_write_vt( buffer2 );
    }

    VIDEO_SetNextFramebuffer( fb );
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();	
    //VIDEO_WaitVSync();
}

/*
 * Move the current selection in the menu by the specified step count
 *
 * menu     The current menu
 * steps    The number of steps to move the selection
 */
void wii_menu_move( TREENODE *menu, s16 steps )
{	
    // TODO: When paging, need to take into consideration non-visible items

    if( !menu ) return;

    s16 new_idx = menu_cur_idx;

    // Attempt to move to the new location
    while( 1 )
    {
        s16 prev_idx = new_idx;
        new_idx += steps;

        if( new_idx >= menu->child_count )
        {
            new_idx = menu->child_count - 1;
        }		

        if( new_idx < 0 )
        {
            new_idx = 0;
        }

        if( new_idx < menu->child_count )
        {
            // Make sure the node can be selected
            TREENODE* curchild = menu->children[new_idx];
            if( ( !wii_menu_is_node_selectable( curchild ) ) &&
                prev_idx != new_idx )
            {
                steps = steps > 0 ? 1 : -1;
            }
            else
            {
                break;
            } 
        }
        else
        {
            break;
        }
    }

    // Is the new location valid? If so make updates.
    if( new_idx >= 0 && 
        new_idx < menu->child_count &&
        wii_menu_is_node_selectable( menu->children[new_idx] ) )
    {
        menu_cur_idx = new_idx;

        if( menu_cur_idx < menu_start_idx )
        {
            menu_start_idx = menu_cur_idx;
        }
        else
        {
            s16 min_start = menu_cur_idx - MENU_PAGE_SIZE + 1;
            if( min_start > menu_start_idx )
            {
                menu_start_idx = min_start;
            }
        }
    }
}

#define DELAY_FRAMES 6
#define DELAY_STEP 1
#define DELAY_MIN 0

/*
 * Displays the menu 
 */
static void wii_menu_show()
{
    // Allows for incremental speed when scrolling the menu 
    // (Scrolls faster the longer the directional pad is held)
    s16 delay_frames = -1;
    s16 delay_factor = -1;

    while( !quit )
    {		
        // Scan the Wii and Gamecube controllers
        WPAD_ScanPads();
        PAD_ScanPads();        

        // Check the state of the controllers
        u32 down = WPAD_ButtonsDown( 0 );
        u32 held = WPAD_ButtonsHeld( 0 );
        u32 gcDown = PAD_ButtonsDown( 0 );
        u32 gcHeld = PAD_ButtonsHeld( 0 );

        // Analog controls
        expansion_t exp;
	    WPAD_Expansion( 0, &exp );        
        float expX = wii_exp_analog_val( &exp, true );
        float expY = wii_exp_analog_val( &exp, false );
        s8 gcX = PAD_StickX( 0 );
        s8 gcY = PAD_StickY( 0 );

        // Classic or Nunchuck?
        bool isClassic = ( exp.type == WPAD_EXP_CLASSIC );

        TREENODE *menu = 
            wii_menu_stack_head >= 0 ?			
            wii_menu_stack[wii_menu_stack_head] : NULL;

        if( redraw_menu )
        {
            redraw_menu = 0;
            wii_menu_render( menu );
        }

        if( menu )
        {                    
            wii_menu_update( menu );

            if( ( ( held & (
                    WII_BUTTON_LEFT | WII_BUTTON_RIGHT | 
                    WII_BUTTON_DOWN | WII_BUTTON_UP |
                    ( isClassic ? 
                        ( WII_CLASSIC_BUTTON_LEFT | WII_CLASSIC_BUTTON_UP ) : 0 ) 
                            ) ) == 0 ) &&
                ( ( gcHeld & (
                    GC_BUTTON_LEFT | GC_BUTTON_RIGHT |
                    GC_BUTTON_DOWN | GC_BUTTON_UP ) ) == 0 ) &&
                ( !wii_analog_left( expX, gcX ) &&
                  !wii_analog_right( expX, gcX ) &&
                  !wii_analog_up( expY, gcY )&&
                  !wii_analog_down( expY, gcY ) ) )
            {
                delay_frames = -1;
                delay_factor = -1;
            }
            else
            {
                if( delay_frames < 0 )
                {
                    if( ( held & ( WII_BUTTON_LEFT | 
                            ( isClassic ? WII_CLASSIC_BUTTON_LEFT : 0 ) ) ) || 
                        ( gcHeld & GC_BUTTON_LEFT ) ||                       
                        wii_analog_left( expX, gcX ) )
                    {
                        wii_menu_move( menu, -MENU_PAGE_SIZE );
                        redraw_menu = 1;
                    }
                    else if( ( held & WII_BUTTON_RIGHT ) ||
                        ( gcHeld & GC_BUTTON_RIGHT ) ||
                        wii_analog_right( expX, gcX ) )
                    {
                        wii_menu_move( menu, MENU_PAGE_SIZE );
                        redraw_menu = 1;
                    }
                    else if( ( held & WII_BUTTON_DOWN ) ||
                        ( gcHeld & GC_BUTTON_DOWN ) ||
                        wii_analog_down( expY, gcY ) )
                    {
                        wii_menu_move( menu, 1 );
                        redraw_menu = 1;
                    }
                    else if( ( held & ( WII_BUTTON_UP | 
                            ( isClassic ? WII_CLASSIC_BUTTON_UP : 0 ) ) ) || 
                        ( gcHeld & GC_BUTTON_UP ) ||
                        wii_analog_up( expY, gcY ) )
                    {
                        if( menu )
                        {
                            wii_menu_move( menu, -1 );						
                            redraw_menu = 1;
                        }
                    }

                    delay_frames = 
                        DELAY_FRAMES - (DELAY_STEP * ++delay_factor);

                    if( delay_frames < DELAY_MIN ) 
                    {
                        delay_frames = DELAY_MIN;
                    }
                }
                else
                {
                    delay_frames--;
                }
            }
           
            if( ( down & ( WII_BUTTON_A | 
                    ( isClassic ? 
                        WII_CLASSIC_BUTTON_A : WII_NUNCHUK_BUTTON_A ) ) ) || 
                ( gcDown & GC_BUTTON_A ) )
            {	
                wii_menu_select_node( menu->children[menu_cur_idx] );
                redraw_menu = 1;            
            }
            if( ( down & ( WII_BUTTON_B | 
                    ( isClassic ? 
                        WII_CLASSIC_BUTTON_B : WII_NUNCHUK_BUTTON_B ) ) ) || 
                ( gcDown & GC_BUTTON_B ) )
            {
                wii_menu_pop();
                redraw_menu = 1;
            }
        }
        
        if( ( down & WII_BUTTON_HOME ) ||
            ( gcDown & GC_BUTTON_HOME ) || 
            wii_hw_button )
        {
            quit = 1;
        }

        VIDEO_WaitVSync();
    }
}

/*
 * Loads the resource for the menu
 */
static void wii_load_resources()
{
    //
    // Load the about image (header)
    //

    char about_loc[WII_MAX_PATH];
    wii_get_app_relative( "about.png", about_loc );

    IMGCTX about_ctx = PNGU_SelectImageFromDevice( about_loc );	

    if( about_ctx != NULL )
    {
        if( PNGU_GetImageProperties( about_ctx, &about_props ) != PNGU_OK )
        {
            PNGU_ReleaseImageContext( about_ctx );
            about_ctx = NULL;
        }
        else
        {
            u32 about_buff_size = 
                about_props.imgWidth * about_props.imgHeight * 2;
            about_buff = (u32*)malloc( about_buff_size );

            PNGU_DecodeToYCbYCr(
                about_ctx, 
                about_props.imgWidth, 
                about_props.imgHeight, 
                about_buff,
                0
            );

            PNGU_ReleaseImageContext( about_ctx );
            about_ctx = NULL;
        }
    }

    //
    // The root menu
    //

    menu_root = wii_create_tree_node( NODETYPE_ROOT, "root" );

    TREENODE* child = NULL;
    child = wii_create_tree_node( NODETYPE_START_EMULATION, "Start emulator" );
    wii_add_child( menu_root, child );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( menu_root, child );

    child = wii_create_tree_node( NODETYPE_SCALE, 
        "Screen size  " );
    wii_add_child( menu_root, child );

    child = wii_create_tree_node( NODETYPE_PADDLE_MODE, 
        "Paddle mode  " );
    wii_add_child( menu_root, child );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( menu_root, child );

    TREENODE *controls = wii_create_tree_node( NODETYPE_CONTROLS, 
        "Controls" );
    wii_add_child( menu_root, controls );

    //
    // The controls menu
    //

    child = wii_create_tree_node( NODETYPE_PADDLE_ROLL_SENSITIVITY, 
        "Paddle roll sensitivity    " );
    wii_add_child( controls, child );

    child = wii_create_tree_node( NODETYPE_PADDLE_ROLL_CENTER, 
        "Paddle roll center         " );
    wii_add_child( controls, child );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( controls, child );

    child = wii_create_tree_node( NODETYPE_PADDLE_IR_SENSITIVITY, 
        "Paddle IR sensitivity      " );
    wii_add_child( controls, child );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( controls, child );

    child = wii_create_tree_node( NODETYPE_PADDLE_ANALOG_SENSITIVITY, 
        "Paddle analog sensitivity  " );
    wii_add_child( controls, child );

    child = wii_create_tree_node( NODETYPE_PADDLE_ANALOG_AXIS, 
        "Paddle analog axis         " );
    wii_add_child( controls, child );

    TREENODE *advanced = wii_create_tree_node( NODETYPE_ADVANCED, 
        "Advanced" );
    wii_add_child( menu_root, advanced );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( menu_root, child );

    wii_menu_push( menu_root );	

    //
    // The advanced menu
    //

    child = wii_create_tree_node( NODETYPE_DEBUG_MODE, 
        "Debug mode        " );
    wii_add_child( advanced, child );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( advanced, child );

    child = wii_create_tree_node( NODETYPE_GAMMA, 
        "Adjust gamma      " );
    wii_add_child( advanced, child );

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( advanced, child );

    child = wii_create_tree_node( NODETYPE_VSYNC, 
        "Vertical sync     " );
    wii_add_child( advanced, child );   

    child = wii_create_tree_node( NODETYPE_FORCE_FPS, 
        "Force FPS         " );
    wii_add_child( advanced, child );   

    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( advanced, child );

    child = wii_create_tree_node( NODETYPE_DISPLAY_ROM_INFO, 
        "Display ROM info  " );
    wii_add_child( advanced, child );   


#if 0
    child = wii_create_tree_node( NODETYPE_SPACER, "" );
    wii_add_child( advanced, child );

    child = wii_create_tree_node( NODETYPE_TEST_FPS, 
        "Test FPS       " );
    wii_add_child( advanced, child );   
#endif
}

/*
 * Frees the menu resources 
 */
static void wii_free_resources()
{
    //
    // Probably completely unnecessary but seems like a good time
    //

    if( about_buff != NULL )
    {
        free( about_buff );
        about_buff = NULL;
    }

    if( menu_root != NULL )
    {
        wii_free_node( menu_root );
        menu_root = NULL;
    }
}

/*
 * Swaps the frame buffers
 */
void wii_swap_frame_buffers()
{
    if( wii_vsync == VSYNC_DOUBLE_BUFF )
    {
        cur_xfb ^= 1;
        frame_buffer = wii_xfb[cur_xfb];
    }
}

/*
 * Flush and syncs the video 
 */
void wii_flush_and_sync_video()
{     
    if( wii_vsync == VSYNC_DOUBLE_BUFF )
    {
        VIDEO_SetNextFramebuffer( wii_xfb[cur_xfb] );
        VIDEO_SetBlack(FALSE);
        VIDEO_Flush();	
    }

    if( wii_vsync == VSYNC_ENABLED ||
        wii_vsync == VSYNC_DOUBLE_BUFF )
    {
        VIDEO_WaitVSync();
    }
}

/*
 * Main for Wii2600
 */
int main( int argc, char *argv[] )
{
    main_argc = argc;
    main_argv = argv;

    // Mount the SD Card
    wii_mount_sd();

    // Capture the path we were launched with
    wii_set_app_path( argc, argv );

    // Register the controllers
    WPAD_Init();
    PAD_Init();

    // Set the hardware callbacks
    wii_register_hw_buttons();

    // Clear the stack
    memset( &wii_menu_stack, 0, sizeof(wii_menu_stack) );

    // Load and read info for our menu
    wii_load_resources();
    wii_read_config();

    // Startup the SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
    {
        fprintf( stderr, "FAILED : Unable to init SDL: %s\n", SDL_GetError() );
        exit(EXIT_FAILURE);
    }

    // Add a new frame buffer to the existing SDL frame buffer so we can double 
    // buffer out menu display
    wii_xfb[0] = frame_buffer;
    wii_xfb[1] = (u32*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(display_mode));	

    // Test for PAL/NTSC
    wii_test_pal();

    // Set the vsync based on whether or not we are PAL or NTSC
    if( wii_first_run )
    {
        wii_set_vsync( !is_pal );
    }
    wii_first_run = 0;

    // Show the menu (starts the menu loop)
    wii_menu_show();

    // We be done, write the config settings, free resources and exit
    wii_write_config();
    wii_free_resources();    
    SDL_Quit();

    if( wii_hw_button )
    {
        // They pressed a HW button, reset based on its type
	    SYS_ResetSystem( wii_hw_button, 0, 0 );
	}
    else if( !!*(u32*)0x80001800 ) 
    {
        // Were we launched via HBC?
        exit(1);
    }
    else
    {
        // Wii channel support
        SYS_ResetSystem( SYS_RETURNTOMENU, 0, 0 );
    }

    return 0;
}
