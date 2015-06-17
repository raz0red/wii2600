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

#ifndef WII_INPUT_H
#define WII_INPUT_H

#include <wiiuse/wpad.h>

// UI Navigation, Standard Buttons
#define WII_BUTTON_LEFT ( WPAD_BUTTON_LEFT )
#define WII_CLASSIC_BUTTON_LEFT ( WPAD_CLASSIC_BUTTON_LEFT )
#define GC_BUTTON_LEFT ( PAD_BUTTON_LEFT )
#define WII_BUTTON_RIGHT ( WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT )
#define GC_BUTTON_RIGHT ( PAD_BUTTON_RIGHT )
#define WII_BUTTON_UP ( WPAD_BUTTON_UP  )
#define WII_CLASSIC_BUTTON_UP ( WPAD_CLASSIC_BUTTON_UP  )
#define GC_BUTTON_UP ( PAD_BUTTON_UP )
#define WII_BUTTON_DOWN ( WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN  )
#define GC_BUTTON_DOWN ( PAD_BUTTON_DOWN )
#define WII_BUTTON_ENTER ( WPAD_BUTTON_A )
#define WII_CLASSIC_BUTTON_ENTER ( WPAD_CLASSIC_BUTTON_A )
#define WII_NUNCHUK_BUTTON_ENTER ( WPAD_NUNCHUK_BUTTON_C )
#define GC_BUTTON_ENTER ( PAD_BUTTON_A )
#define WII_BUTTON_ESC ( WPAD_BUTTON_B )
#define WII_CLASSIC_BUTTON_ESC ( WPAD_CLASSIC_BUTTON_B )
#define WII_NUNCHUK_BUTTON_ESC ( WPAD_NUNCHUK_BUTTON_Z )
#define GC_BUTTON_ESC ( PAD_BUTTON_B )
#define WII_BUTTON_HOME ( WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME )
#define GC_BUTTON_HOME ( PAD_TRIGGER_Z )
#define WII_TAB_FORWARD ( WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_FULL_R | WPAD_CLASSIC_BUTTON_PLUS )
#define GC_TAB_FORWARD ( PAD_TRIGGER_R )
#define WII_TAB_BACKWARD ( WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_FULL_L | WPAD_CLASSIC_BUTTON_MINUS )
#define GC_TAB_BACKWARD ( PAD_TRIGGER_L )

// In game controls
#define WII_BUTTON_ATARI_RESET ( WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS )
#define GC_BUTTON_ATARI_RESET ( PAD_BUTTON_START )
#define WII_BUTTON_ATARI_SELECT ( WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS )
#define GC_BUTTON_ATARI_SELECT ( PAD_TRIGGER_L )
#define WII_BUTTON_ATARI_FIRE ( WPAD_BUTTON_2 )
#define GC_BUTTON_ATARI_FIRE ( PAD_BUTTON_A )
#define WII_CLASSIC_ATARI_FIRE ( WPAD_CLASSIC_BUTTON_A )
#define WII_NUNCHECK_ATARI_FIRE ( WPAD_NUNCHUK_BUTTON_C | WPAD_NUNCHUK_BUTTON_Z )
#define WII_BUTTON_ATARI_UP ( WPAD_BUTTON_RIGHT )
#define GC_BUTTON_ATARI_UP ( PAD_BUTTON_UP )
#define WII_CLASSIC_ATARI_UP ( WPAD_CLASSIC_BUTTON_UP )
#define WII_BUTTON_ATARI_DOWN ( WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_DOWN )
#define GC_BUTTON_ATARI_DOWN ( PAD_BUTTON_DOWN )
#define WII_BUTTON_ATARI_RIGHT ( WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_RIGHT )
#define GC_BUTTON_ATARI_RIGHT ( PAD_BUTTON_RIGHT )
#define WII_BUTTON_ATARI_LEFT ( WPAD_BUTTON_UP )
#define WII_CLASSIC_ATARI_LEFT ( WPAD_CLASSIC_BUTTON_LEFT )
#define GC_BUTTON_ATARI_LEFT ( PAD_BUTTON_LEFT )
#define WII_BUTTON_ATARI_CMD_MENU ( WPAD_BUTTON_1 | WPAD_CLASSIC_BUTTON_Y )
#define WII_BUTTON_ATARI_CMD_MENU_CLOSE ( WII_BUTTON_ATARI_CMD_MENU | WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B )
#define GC_BUTTON_ATARI_CMD_MENU ( PAD_BUTTON_Y )
#define GC_BUTTON_ATARI_CMD_MENU_CLOSE ( GC_BUTTON_ATARI_CMD_MENU | PAD_BUTTON_B )
#define WII_BUTTON_ATARI_BW ( WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_FULL_R )
#define GC_BUTTON_ATARI_BW ( PAD_TRIGGER_R )

// Paddle-specific in game controls
#define WII_BUTTON_ATARI_PADDLE_FIRE ( WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_2 | WII_CLASSIC_ATARI_FIRE | WII_NUNCHECK_ATARI_FIRE )
#define GC_BUTTON_ATARI_PADDLE_FIRE ( GC_BUTTON_ATARI_FIRE )

// General
#define WII_BUTTON_A WII_BUTTON_ENTER
#define WII_CLASSIC_BUTTON_A WII_CLASSIC_BUTTON_ENTER
#define WII_NUNCHUK_BUTTON_A WII_NUNCHUK_BUTTON_ENTER
#define GC_BUTTON_A GC_BUTTON_ENTER
#define WII_BUTTON_B WII_BUTTON_ESC
#define WII_CLASSIC_BUTTON_B WII_CLASSIC_BUTTON_ESC
#define WII_NUNCHUK_BUTTON_B WII_NUNCHUK_BUTTON_ESC
#define GC_BUTTON_B GC_BUTTON_ESC

// Paddles
#define WIIMOTE_ROLL_SENSITIVITY_LEVELS 10
#define WIIMOTE_ROLL_SENSITIVITY_DEFAULT 5
#define WIIMOTE_ROLL_MIN ( 10.0 )
#define WIIMOTE_ROLL_MAX ( 120.0 )
#define WIIMOTE_ROLL_CENTER_ADJ_VALUE_DEFAULT 0
#define WIIMOTE_ROLL_CENTER_ADJ_INCREMENT 2.35
#define WIIMOTE_ROLL_CENTER_ADJ_MID 14.0  

#define WIIMOTE_IR_CENTER 512.0
#define WIIMOTE_IR_SENSITIVITY_LEVELS 10
#define WIIMOTE_IR_SENSITIVITY_DEFAULT 5
#define WIIMOTE_IR_SENSITIVITY_INCREMENT 51.0
#define WIIMOTE_IR_CENTER_ADJUST 25

#define PADDLE_ANALOG_SENSITIVITY_LEVELS 10
#define PADDLE_ANALOG_SENSITIVITY_DEFAULT 1

#define WII_ANALOG_MIN -128.0
#define WII_ANALOG_MAX 128.0
#define WII_ANALOG_CENTER_ADJUST 15
#define WII_ANALOG_SENSITIVITY_INCREMENT 12.8

#define GC_ANALOG_MIN -100.0
#define GC_ANALOG_MAX 100.0
#define GC_ANALOG_CENTER_ADJUST 30
#define GC_ANALOG_SENSITIVITY_INCREMENT 10.0

#define WII_PADDLE_MODE_MOTE_ROLL 0
#define WII_PADDLE_MODE_MOTE_IR 1
#define WII_PADDLE_MODE_ANALOG 2

/**
 * Returns the analog value for the joystick (-128.0 to 128.0)
 * 
 * exp - The expansion (Nunchuk or Classic)
 * isX - Whether to get the X or Y axis value 
 */
extern float wii_exp_analog_val( const expansion_t* exp, bool isX );

extern inline bool wii_analog_right( float expX, s8 gcX ) { return expX > 60 || gcX > 46; }
extern inline bool wii_analog_left( float expX, s8 gcX ) { return expX < -60 || gcX < -46; }
extern inline bool wii_analog_up( float expY, s8 gcY ) { return expY > 70 || gcY > 54; }
extern inline bool wii_analog_down( float expY, s8 gcY ) { return expY < -70 || gcY < -54; }

/** The mode for the paddles */
extern int wii_paddle_mode;
/** Sensitivity for paddle Wiimote "roll" mode */
extern int wii_paddle_roll_sensitivity;
/** Sensitivity for paddle Wiimote "center" mode */
extern int wii_paddle_roll_center;
/** Sensitivity for paddle Wiimote "IR" mode */
extern int wii_paddle_ir_sensitivity;
/** Sensitivity for analog controls */
extern int wii_paddle_analog_sensitivity;
/** Whether x-asix for analog controls */
extern BOOL wii_paddle_analog_xaxis;


#endif

