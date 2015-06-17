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

#include <stdio.h>
#include <string.h>

#include "wii_main.hxx"
#include "wii_file_io.hxx"
#include "wii_config.hxx"
#include "wii_util.hxx"
#include "wii_app.hxx"
#include "wii_util.hxx"
#include "wii_input.hxx"

/*
 * Read the configuration file
 *
 * return   Was the read success?
 */
BOOL wii_read_config()
{
    char buff[512];

    char config_loc[WII_MAX_PATH];
    wii_get_app_relative( WII_CONFIG, config_loc );

    FILE *fp;
    fp = fopen( config_loc, "r" );
    if (fp == NULL) 
    {	
        // Remount the SD Card
        wii_remount_sd();

        return FALSE;		
    }

    while( fgets( buff, sizeof( buff ), fp ) ) 
    {
        char *ptr;
        Util_chomp( buff );
        ptr = strchr( buff, '=' );
        if( ptr != NULL ) 
        {
            *ptr++ = '\0';
            Util_trim( buff );
            Util_trim( ptr );

            if( strcmp( buff, "SCREEN_SIZE" ) == 0 )
            {
                wii_scale = Util_sscandec( ptr );
            }
            else if( strcmp( buff, "VSYNC" ) == 0 )
            {
                wii_vsync = Util_sscandec( ptr );
            }
            else if ( strcmp( buff, "DEBUG" ) == 0 )
            {
                wii_debug = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "FORCE_FPS" ) == 0 )
            {
                wii_force_fps = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "GAMMA" ) == 0 )
            {
                wii_gamma = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "FIRST_RUN" ) == 0 )
            {
                wii_first_run = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "PADDLE_MODE" ) == 0 )
            {
                wii_paddle_mode = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "PADDLE_ROLL_SENSITIVITY" ) == 0 )
            {
                wii_paddle_roll_sensitivity = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "PADDLE_ROLL_CENTER" ) == 0 )
            {
                wii_paddle_roll_center = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "PADDLE_IR_SENSITIVITY" ) == 0 )
            {
                wii_paddle_ir_sensitivity = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "DISPLAY_ROM_INFO" ) == 0 )
            {
                wii_display_rom_info = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "PADDLE_ANALOG_SENSITIVITY" ) == 0 )
            {
                wii_paddle_analog_sensitivity = Util_sscandec( ptr );				
            }
            else if ( strcmp( buff, "PADDLE_ANALOG_AXIS" ) == 0 )
            {
                wii_paddle_analog_xaxis = Util_sscandec( ptr );				
            }
        }
    }

    fclose(fp);

    // Remount the SD Card
    wii_remount_sd();

    return TRUE;
}

/*
 * Write the configuration file
 *
 * return   Was the write success?
 */
BOOL wii_write_config()
{
    char config_loc[WII_MAX_PATH];
    wii_get_app_relative( WII_CONFIG, config_loc );

    FILE *fp;
    fp = fopen( config_loc, "w" );
    if( fp == NULL ) 
    {
        // Remount the SD Card
        wii_remount_sd();

        return FALSE;
    }

    fprintf( fp, "SCREEN_SIZE=%d\n", wii_scale );
    fprintf( fp, "VSYNC=%d\n", wii_vsync );
    fprintf( fp, "DEBUG=%d\n", wii_debug );
    fprintf( fp, "FORCE_FPS=%d\n", wii_force_fps );
    fprintf( fp, "GAMMA=%d\n", wii_gamma );
    fprintf( fp, "FIRST_RUN=%d\n", wii_first_run );
    fprintf( fp, "PADDLE_MODE=%d\n", wii_paddle_mode );
    fprintf( fp, "PADDLE_ROLL_SENSITIVITY=%d\n", wii_paddle_roll_sensitivity );
    fprintf( fp, "PADDLE_ROLL_CENTER=%d\n", wii_paddle_roll_center );
    fprintf( fp, "PADDLE_IR_SENSITIVITY=%d\n", wii_paddle_ir_sensitivity );
    fprintf( fp, "DISPLAY_ROM_INFO=%d\n", wii_display_rom_info );
    fprintf( fp, "PADDLE_ANALOG_SENSITIVITY=%d\n", wii_paddle_analog_sensitivity );
    fprintf( fp, "PADDLE_ANALOG_AXIS=%d\n", wii_paddle_analog_xaxis );    
    
    fclose(fp);

    // Remount the SD Card
    wii_remount_sd();

    return TRUE;
}


