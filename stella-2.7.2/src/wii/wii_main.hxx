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

#ifndef WII_MAIN_H
#define WII_MAIN_H

#include <gccore.h>
#include <sys/dir.h>
#include <unistd.h>

/*
 * The different types of nodes in the menu
 */
enum NODETYPE
{
    NODETYPE_ROOT,
    NODETYPE_SCALE,
    NODETYPE_SPACER,
    NODETYPE_START_EMULATION,
    NODETYPE_ADVANCED,
    NODETYPE_DEBUG_MODE,
    NODETYPE_VSYNC,
    NODETYPE_FORCE_FPS,
    NODETYPE_GAMMA,
    NODETYPE_TEST_FPS,
    NODETYPE_PADDLE_MODE,    
    NODETYPE_CONTROLS,
    NODETYPE_PADDLE_ROLL_SENSITIVITY,
    NODETYPE_PADDLE_ROLL_CENTER,
    NODETYPE_PADDLE_IR_SENSITIVITY,
    NODETYPE_DISPLAY_ROM_INFO,
    NODETYPE_PADDLE_ANALOG_SENSITIVITY,
    NODETYPE_PADDLE_ANALOG_AXIS
};

struct treenode;
typedef struct treenode *TREENODEPTR;

/*
 * Simple hierarchical menu structure
 */
typedef struct treenode
{
    char *name;
    enum NODETYPE node_type;
    TREENODEPTR *children;
    u16 child_count;
    u16 max_children;
} TREENODE;

#define WII_SD_CARD "sd:"
#define WII_BASE_APP_DIR WII_SD_CARD "/apps/wii2600/"
#define WII_FILES_DIR WII_SD_CARD "/wii2600"
#define WII_CONFIG "wii2600.conf"

#define WII_MAX_PATH MAXPATHLEN
#define WII_MENU_BUFF_SIZE 256

// Whether to display debug info (FPS, etc.)
extern short wii_debug;
// The most recent status message
extern char wii_status_message[];
// Hardware buttons (reset, power, etc.)
extern u8 wii_hw_button;
// The stack containing the menus the user has navigated
extern TREENODEPTR wii_menu_stack[4];
// The head of the menu stack
extern s8 wii_menu_stack_head;
// Two framebuffers (double buffered)
extern u32 *wii_xfb[2];

// The scale
extern int wii_scale;

// The vsync mode
extern int wii_vsync;

// Adjust the gamme
extern int wii_gamma;

// Force fps
extern int wii_force_fps;

// Whether to display rom info
extern BOOL wii_display_rom_info;

// Whether this is the first time we have run
extern BOOL wii_first_run;

// The frame buffer (from SDL)
extern u32 *frame_buffer;
// The display mode (from SDL)
extern GXRModeObj *display_mode;	

/*
 * Resets the menu indexes when an underlying menu in change (push/pop)
 */
extern void wii_menu_reset_indexes();

/*
 * Move the current selection in the menu by the specified step count
 *
 * menu     The current menu
 * steps    The number of steps to move the selection
 */
extern void wii_menu_move( TREENODE *menu, s16 steps );

/*
 * Pops the latest menu off of the menu stack (occurs when the user leaves 
 * the current menu.
 *
 * return   The pop'd menu
 */
extern TREENODE* wii_menu_pop();

/*
 * Swaps the frame buffers
 */
extern void wii_swap_frame_buffers();

/*
 * Flush and syncs the video 
 */
extern void wii_flush_and_sync_video();

// vsync modes
#define VSYNC_DISABLED 0
#define VSYNC_ENABLED 1
#define VSYNC_DOUBLE_BUFF 2

#endif
