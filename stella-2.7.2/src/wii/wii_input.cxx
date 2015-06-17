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
#include <math.h>
#include "wii_input.hxx"

#define PI 3.14159265f

/** The mode for the paddles */
int wii_paddle_mode = WII_PADDLE_MODE_MOTE_ROLL;
/** Sensitivity for paddle Wiimote "roll" mode */
int wii_paddle_roll_sensitivity = WIIMOTE_ROLL_SENSITIVITY_DEFAULT;
/** Sensitivity for paddle Wiimote "center" mode */
int wii_paddle_roll_center = WIIMOTE_ROLL_CENTER_ADJ_VALUE_DEFAULT;
/** Sensitivity for paddle Wiimote "IR" mode */
int wii_paddle_ir_sensitivity = WIIMOTE_IR_SENSITIVITY_DEFAULT;
/** Sensitivity for paddle analog mode */
int wii_paddle_analog_sensitivity = PADDLE_ANALOG_SENSITIVITY_DEFAULT;
/** Whether x-asix for analog controls */
extern BOOL wii_paddle_analog_xaxis = TRUE;

/**
 * Returns the analog value for the joystick (-128.0 to 128.0)
 * 
 * exp - The expansion (Nunchuk or Classic)
 * isX - Whether to get the X or Y axis value 
 */
float wii_exp_analog_val( const expansion_t* exp, bool isX )
{
    float mag = 0.0;
    float ang = 0.0;

    if( exp->type == WPAD_EXP_CLASSIC )
    {
        mag = exp->classic.ljs.mag;
        ang = exp->classic.ljs.ang;
    }
    else if ( exp->type == WPAD_EXP_NUNCHUK )
    {
        mag = exp->nunchuk.js.mag;
        ang = exp->nunchuk.js.ang;
    }
    else
    {
        return 0.0;
    }

    if( mag > 1.0 ) mag = 1.0;
    else if( mag < -1.0 ) mag = -1.0;
    double val = ( isX ?
        mag * sin( PI * ang / 180.0f ) :
        mag * cos( PI * ang / 180.0f ) );

    return val * 128.0f;
}

