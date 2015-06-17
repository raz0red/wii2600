//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Paddles.cxx,v 1.16 2009/01/01 18:13:36 stephena Exp $
//============================================================================

#define TRIGMAX 240
#define TRIGMIN 1

#include "Event.hxx"
#include "Paddles.hxx"
#ifdef WII
#include <wiiuse/wpad.h>
#include "wii_main.hxx"
#include "wii_input.hxx"
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Paddles::Paddles(Jack jack, const Event& event, const System& system, bool swap)
  : Controller(jack, event, system, Controller::Paddles)
{

#ifdef WII
    initWiiSettings( swap );
#endif

  // Swap the paddle events, from paddle 0 <=> 1 and paddle 2 <=> 3
  // Also consider whether this is the left or right port
  if(myJack == Left)
  {
    if(!swap)
    {
      myP0AxisValue  = Event::SALeftAxis0Value;
      myP0DecEvent1  = Event::PaddleZeroDecrease;
      myP0DecEvent2  = Event::JoystickZeroRight;
      myP0IncEvent1  = Event::PaddleZeroIncrease;
      myP0IncEvent2  = Event::JoystickZeroLeft;
      myP0FireEvent1 = Event::PaddleZeroFire;
      myP0FireEvent2 = Event::JoystickZeroFire1;

      myP1AxisValue  = Event::SALeftAxis1Value;
      myP1DecEvent1  = Event::PaddleOneDecrease;
      myP1DecEvent2  = Event::JoystickZeroUp;
      myP1IncEvent1  = Event::PaddleOneIncrease;
      myP1IncEvent2  = Event::JoystickZeroDown;
      myP1FireEvent1 = Event::PaddleOneFire;
      myP1FireEvent2 = Event::JoystickZeroFire3;

      if(_MOUSEX_PADDLE < 0)  _MOUSEX_PADDLE = 0;
    }
    else
    {
      myP0AxisValue  = Event::SALeftAxis1Value;
      myP0DecEvent1  = Event::PaddleOneDecrease;
      myP0DecEvent2  = Event::JoystickZeroUp;
      myP0IncEvent1  = Event::PaddleOneIncrease;
      myP0IncEvent2  = Event::JoystickZeroDown;
      myP0FireEvent1 = Event::PaddleOneFire;
      myP0FireEvent2 = Event::JoystickZeroFire3;

      myP1AxisValue  = Event::SALeftAxis0Value;
      myP1DecEvent1  = Event::PaddleZeroDecrease;
      myP1DecEvent2  = Event::JoystickZeroRight;
      myP1IncEvent1  = Event::PaddleZeroIncrease;
      myP1IncEvent2  = Event::JoystickZeroLeft;
      myP1FireEvent1 = Event::PaddleZeroFire;
      myP1FireEvent2 = Event::JoystickZeroFire1;

      if(_MOUSEX_PADDLE < 0)  _MOUSEX_PADDLE = 1;
    }
  }
  else
  {
    if(!swap)
    {
      myP0AxisValue  = Event::SARightAxis0Value;
      myP0DecEvent1  = Event::PaddleTwoDecrease;
      myP0DecEvent2  = Event::JoystickOneRight;
      myP0IncEvent1  = Event::PaddleTwoIncrease;
      myP0IncEvent2  = Event::JoystickOneLeft;
      myP0FireEvent1 = Event::PaddleTwoFire;
      myP0FireEvent2 = Event::JoystickOneFire1;

      myP1AxisValue  = Event::SARightAxis1Value;
      myP1DecEvent1  = Event::PaddleThreeDecrease;
      myP1DecEvent2  = Event::JoystickOneUp;
      myP1IncEvent1  = Event::PaddleThreeIncrease;
      myP1IncEvent2  = Event::JoystickOneDown;
      myP1FireEvent1 = Event::PaddleThreeFire;
      myP1FireEvent2 = Event::JoystickOneFire3;

      if(_MOUSEX_PADDLE < 0)  _MOUSEX_PADDLE = 0;
    }
    else
    {
      myP0AxisValue  = Event::SARightAxis1Value;
      myP0DecEvent1  = Event::PaddleThreeDecrease;
      myP0DecEvent2  = Event::JoystickOneUp;
      myP0IncEvent1  = Event::PaddleThreeIncrease;
      myP0IncEvent2  = Event::JoystickOneDown;
      myP0FireEvent1 = Event::PaddleThreeFire;
      myP0FireEvent2 = Event::JoystickOneFire3;

      myP1AxisValue  = Event::SARightAxis0Value;
      myP1DecEvent1  = Event::PaddleTwoDecrease;
      myP1DecEvent2  = Event::JoystickOneRight;
      myP1IncEvent1  = Event::PaddleTwoIncrease;
      myP1IncEvent2  = Event::JoystickOneLeft;
      myP1FireEvent1 = Event::PaddleTwoFire;
      myP1FireEvent2 = Event::JoystickOneFire1;

      if(_MOUSEX_PADDLE < 0)  _MOUSEX_PADDLE = 1;
    }
  }

  // Digital pins 1, 2 and 6 are not connected
  myDigitalPinState[One] =
  myDigitalPinState[Two] =
  myDigitalPinState[Six] = true;

  // Digital emulation of analog paddle movement
  myKeyRepeat0 = myPaddleRepeat0 = myKeyRepeat1 = myPaddleRepeat1 = 0;

  myCharge[0] = myCharge[1] =
  myLastCharge[0] = myLastCharge[1] = TRIGMAX/2;  // half of maximum paddle charge
  myLeftMotion[0] = myLeftMotion[1] = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Paddles::~Paddles()
{
}

//#ifdef WII

void Paddles::setRightPaddleOffset( const int offset )
{
    wiiRightPaddleOffset = offset;
}

/**
 * Initializes the settings for Wii paddle support
 *
 * swap - Whether we are swapping controllers
 */
void Paddles::initWiiSettings( const bool swap )
{
    wiiRightPaddleOffset = 2;

    wiiSwap = swap;
    wiiPrevp0 = -1;
    wiiPrevp1 = -1;

    wiiPaddleRange = ((float)TRIGMAX) - TRIGMIN;

    //
    // Wiimote - IR mode
    //

    float wiiIrAdjust = 
        ( WIIMOTE_IR_SENSITIVITY_LEVELS - wii_paddle_ir_sensitivity + 1 ) *
            WIIMOTE_IR_SENSITIVITY_INCREMENT;

    wiiIrMin = WIIMOTE_IR_CENTER - wiiIrAdjust;
    wiiIrMax = WIIMOTE_IR_CENTER + wiiIrAdjust;
    wiiIrRange = wiiIrMax - wiiIrMin;

    //
    // Wiimote - Roll mode
    //

    float rollIncrement = 
        ( WIIMOTE_ROLL_MAX - WIIMOTE_ROLL_MIN ) / WIIMOTE_ROLL_SENSITIVITY_LEVELS;

    wiiRollMax = WIIMOTE_ROLL_MIN + 
        ( ( WIIMOTE_ROLL_SENSITIVITY_LEVELS - wii_paddle_roll_sensitivity ) * rollIncrement );

    wiiRollMin = -wiiRollMax;
    wiiRollRange = wiiRollMax - wiiRollMin;
    wiiRollCenter = 
        wiiRollRange / 
        ( WIIMOTE_ROLL_CENTER_ADJ_MID - 
            ( WIIMOTE_ROLL_CENTER_ADJ_INCREMENT * ( abs( wii_paddle_roll_center ) ) ) );

    if( wii_paddle_roll_center < 0 ) 
        wiiRollCenter = -wiiRollCenter;

    wiiRollMax -= wiiRollCenter;
    wiiRollMin -= wiiRollCenter;
    wiiRollRatio = wiiPaddleRange / wiiRollRange;

    // Wii analog controls    
    wiiAnalogMax =
        ( ( PADDLE_ANALOG_SENSITIVITY_LEVELS - wii_paddle_analog_sensitivity + 1 )
            * WII_ANALOG_SENSITIVITY_INCREMENT );
    wiiAnalogMin =  -wiiAnalogMax;
    wiiAnalogRange = wiiAnalogMax - wiiAnalogMin;
    wiiAnalogRatio = ( wiiPaddleRange / wiiAnalogRange );

    // Gamecube analog controls
    gcAnalogMax = 
        ( ( PADDLE_ANALOG_SENSITIVITY_LEVELS - wii_paddle_analog_sensitivity + 1 )
            * GC_ANALOG_SENSITIVITY_INCREMENT );
    gcAnalogMin = -gcAnalogMax;
    gcAnalogRange = gcAnalogMax - gcAnalogMin;
    gcAnalogRatio = ( gcAnalogRange / gcAnalogRange );
}

/**
 * Returns the position of the paddle for the specified 
 * controller
 *
 * prev - The last position for the controller
 * mote - The index of the controller
 */
int Paddles::getWiiPaddlePosition( const int prev, const int mote )
{    
    WPADData *wd = WPAD_Data( mote );	

    if( wii_paddle_mode == WII_PADDLE_MODE_ANALOG )
    {
        if( wd->exp.type == WPAD_EXP_CLASSIC || wd->exp.type == WPAD_EXP_NUNCHUK )
        {
            float x = wii_exp_analog_val( &(wd->exp), wii_paddle_analog_xaxis );

            if( x > wiiAnalogMin && x < wiiAnalogMax )
            {    
                x = ( x - wiiAnalogMin );

                int value = (int)( x * wiiAnalogRatio );
                int retVal = ( TRIGMAX - ( TRIGMIN + value ) ) - WII_ANALOG_CENTER_ADJUST;

                if( retVal < TRIGMIN ) retVal = TRIGMIN;
                else if( retVal > TRIGMAX ) retVal = TRIGMAX;
                return retVal;    
            }
            else
            {
                return prev;
            }
        }
        else
        {
            float x = 
                ( wii_paddle_analog_xaxis  ? PAD_StickX( mote ) : PAD_StickY( mote ) );

            if( x > gcAnalogMin && x < gcAnalogMax )
            {    
                x = ( x - gcAnalogMin );

                int value = (int)( x * gcAnalogRatio );
                int retVal = ( TRIGMAX - ( TRIGMIN + value ) ) - GC_ANALOG_CENTER_ADJUST;

                if( retVal < TRIGMIN ) retVal = TRIGMIN;
                else if( retVal > TRIGMAX ) retVal = TRIGMAX;
                return retVal;    
            }
            else
            {
                return prev;
            }
        }
    }
    else if( wii_paddle_mode == WII_PADDLE_MODE_MOTE_IR )
    {
        if( wd->ir.ax > wiiIrMin && wd->ir.ax < wiiIrMax )
        {    
	        int retVal = 
                TRIGMAX - ( ( ( ( ( float)wd->ir.ax - wiiIrMin ) / wiiIrRange ) * TRIGMAX ) )
                    - WIIMOTE_IR_CENTER_ADJUST;

            if( retVal < TRIGMIN  ) retVal = TRIGMIN;
            else if( retVal > TRIGMAX  ) retVal = TRIGMAX;
            return retVal;
        }
        else
        {
            return 
                ( ( prev < ( ( TRIGMAX / 2 ) - WIIMOTE_IR_CENTER_ADJUST ) ) ? 
                    TRIGMIN : TRIGMAX );
        }
    }
    else if( wii_paddle_mode == WII_PADDLE_MODE_MOTE_ROLL )
    {
        orient_t orient;
        WPAD_Orientation( mote, &orient );

        float roll = orient.roll;
        if( roll > wiiRollMin && roll < wiiRollMax )
        {
            int value = ((int)( ( roll - wiiRollMin ) * wiiRollRatio ));
            int retVal = ( TRIGMAX - ( TRIGMIN + value ) );
            if( retVal < TRIGMIN  ) retVal = TRIGMIN;
            else if( retVal > TRIGMAX  ) retVal = TRIGMAX;

            return retVal;
        }
        else
        {        
            return ( orient.roll > ( -wiiRollCenter ) ) ? TRIGMIN : TRIGMAX ;
        }
    }
}
//#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Paddles::update()
{
    myDigitalPinState[Three] = myDigitalPinState[Four] = true;

#ifdef WII
    int base = ( ( myJack == Left  ) ? 0 : wiiRightPaddleOffset );
    int p0 = wiiSwap ? base + 1 : base;
    int p1 = wiiSwap ? base : base + 1;

    wiiPrevp0 = getWiiPaddlePosition( wiiPrevp0, p0 );
    wiiPrevp1 = getWiiPaddlePosition( wiiPrevp1, p1 );

    myCharge[0] = wiiPrevp0;
    myCharge[1] = wiiPrevp1;	 

    u32 held = WPAD_ButtonsHeld( p0 );
    u32 gcHeld = PAD_ButtonsHeld( p0 );
    if( held & WII_BUTTON_ATARI_PADDLE_FIRE || gcHeld & GC_BUTTON_ATARI_PADDLE_FIRE )
    {
        myDigitalPinState[Four] = false;
    }

    held = WPAD_ButtonsHeld( p1 );
    gcHeld = PAD_ButtonsHeld( p1 );
    if( held & WII_BUTTON_ATARI_PADDLE_FIRE || gcHeld & GC_BUTTON_ATARI_PADDLE_FIRE )
    {
        myDigitalPinState[Three] = false;
    }
#else
  // Digital events (from keyboard or joystick hats & buttons)
  myDigitalPinState[Three] =
    (myEvent.get(myP1FireEvent1) == 0 && myEvent.get(myP1FireEvent2) == 0);
  myDigitalPinState[Four]  =
    (myEvent.get(myP0FireEvent1) == 0 && myEvent.get(myP0FireEvent2) == 0);

  if(myKeyRepeat0)
  {
    myPaddleRepeat0++;
    if(myPaddleRepeat0 > _PADDLE_SPEED)  myPaddleRepeat0 = 2;
  }
  if(myKeyRepeat1)
  {
    myPaddleRepeat1++;
    if(myPaddleRepeat1 > _PADDLE_SPEED)  myPaddleRepeat1 = 2;
  }

  myKeyRepeat0 = 0;
  myKeyRepeat1 = 0;

  if(myEvent.get(myP0DecEvent1) || myEvent.get(myP0DecEvent2))
  {
    myKeyRepeat0 = 1;
    if(myCharge[0] > myPaddleRepeat0)
      myCharge[0] -= myPaddleRepeat0;
  }
  if(myEvent.get(myP0IncEvent1) || myEvent.get(myP0IncEvent2))
  {
    myKeyRepeat0 = 1;
    if((myCharge[0] + myPaddleRepeat0) < TRIGMAX)
      myCharge[0] += myPaddleRepeat0;
  }
  if(myEvent.get(myP1DecEvent1) || myEvent.get(myP1DecEvent2))
  {
    myKeyRepeat1 = 1;
    if(myCharge[1] > myPaddleRepeat1)
      myCharge[1] -= myPaddleRepeat1;
  }
  if(myEvent.get(myP1IncEvent1) || myEvent.get(myP1IncEvent2))
  {
    myKeyRepeat1 = 1;
    if((myCharge[1] + myPaddleRepeat1) < TRIGMAX)
      myCharge[1] += myPaddleRepeat1;
  }

  // Mouse events
  if(myJack == Left && (_MOUSEX_PADDLE == 0 || _MOUSEX_PADDLE == 1))
  {
    // TODO - add infrastructure to map mouse direction to increase or decrease charge
    myCharge[_MOUSEX_PADDLE] -= myEvent.get(Event::MouseAxisXValue);
    if(myCharge[_MOUSEX_PADDLE] < TRIGMIN) myCharge[_MOUSEX_PADDLE] = TRIGMIN;
    if(myCharge[_MOUSEX_PADDLE] > TRIGMAX) myCharge[_MOUSEX_PADDLE] = TRIGMAX;
    if(myEvent.get(Event::MouseButtonValue))
      myDigitalPinState[ourButtonPin[_MOUSEX_PADDLE]] = false;
  }
  else if(myJack == Right && (_MOUSEX_PADDLE == 2 || _MOUSEX_PADDLE == 3))
  {
    // TODO - add infrastructure to map mouse direction to increase or decrease charge
    myCharge[_MOUSEX_PADDLE-2] -= myEvent.get(Event::MouseAxisXValue);
    if(myCharge[_MOUSEX_PADDLE-2] < TRIGMIN) myCharge[_MOUSEX_PADDLE-2] = TRIGMIN;
    if(myCharge[_MOUSEX_PADDLE-2] > TRIGMAX) myCharge[_MOUSEX_PADDLE-2] = TRIGMAX;
    if(myEvent.get(Event::MouseButtonValue))
      myDigitalPinState[ourButtonPin[_MOUSEX_PADDLE-2]] = false;
  }

  // Axis events (possibly use analog values)
  int xaxis = myEvent.get(myP0AxisValue);
  int yaxis = myEvent.get(myP1AxisValue);

  // Filter out jitter by not allowing rapid direction changes
  int charge0 = ((32767 - xaxis) >> 8) & 0xff;
  if(charge0 - myLastCharge[0] > 0)  // we are moving left
  {
    if(!myLeftMotion[0])  // moving right before?
    {
      if(charge0 - myLastCharge[0] <= 4)
      {
        myCharge[0] = myLastCharge[0];
      }
      else
      {
        myCharge[0] = (charge0 + myLastCharge[0]) >> 1;
        myLastCharge[0] = charge0;
        myLeftMotion[0] = 1;
      }
    }
    else
    {
      myCharge[0] = (charge0 + myLastCharge[0]) >> 1;
      myLastCharge[0] = charge0;
    }
  }
  // Filter out jitter by not allowing rapid direction changes
  else if(charge0 - myLastCharge[0] < 0)  // we are moving right
  {
    if(myLeftMotion[0])  // moving left before?
    {
      if(myLastCharge[0] - charge0 <= 4)
      {
        myCharge[0] = myLastCharge[0];
      }
      else
      {
        myCharge[0] = (charge0 + myLastCharge[0]) >> 1;
        myLastCharge[0] = charge0;
        myLeftMotion[0] = 0; 
      }
    }
    else
    {
      myCharge[0] = (charge0 + myLastCharge[0]) >> 1;
      myLastCharge[0] = charge0;
    }
  }

  // Filter out jitter by not allowing rapid direction changes
  int charge1 = ((32767 - yaxis) >> 8) & 0xff;
  if(charge1 - myLastCharge[1] > 0)  // we are moving left
  {
    if(!myLeftMotion[1])  // moving right before?
    {
      if(charge1 - myLastCharge[1] <= 4)
      {
        myCharge[1] = myLastCharge[1];
      }
      else
      {
        myCharge[1] = (charge1 + myLastCharge[1]) >> 1;
        myLastCharge[1] = charge1;
        myLeftMotion[1] = 1; 
      }
    }
    else
    {
      myCharge[1] = (charge1 + myLastCharge[1]) >> 1;
      myLastCharge[1] = charge1;
    }
  }
  // Filter out jitter by not allowing rapid direction changes
  else if(charge1 - myLastCharge[1] < 0)  // we are moving right
  {
    if(myLeftMotion[1])  // moving left before?
    {
      if(myLastCharge[1] - charge1 <= 4)
      {
        myCharge[1] = myLastCharge[1];
      }
      else
      {
        myCharge[1] = (charge1 + myLastCharge[1]) >> 1;
        myLastCharge[1] = charge1;
        myLeftMotion[1] = 0; 
      }
    }
    else
    {
      myCharge[1] = (charge1 + myLastCharge[1]) >> 1;
      myLastCharge[1] = charge1;
    }
  }
#endif
 
  myAnalogPinValue[Five] = (Int32)(1000000 * (myCharge[1] / 255.0));
  myAnalogPinValue[Nine] = (Int32)(1000000 * (myCharge[0] / 255.0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Paddles::setMouseIsPaddle(int number, int dir)
{
  // TODO - make mouse Y axis be actually used in the code above
  if(dir == 0)
    _MOUSEX_PADDLE = number;
  else
    _MOUSEY_PADDLE = number;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Paddles::_PADDLE_SPEED = 6;
int Paddles::_MOUSEX_PADDLE = -1;
int Paddles::_MOUSEY_PADDLE = -1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Controller::DigitalPin Paddles::ourButtonPin[2] = { Four, Three };
