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
// $Id: Paddles.hxx,v 1.15 2009/01/01 18:13:36 stephena Exp $
//============================================================================

#ifndef PADDLES_HXX
#define PADDLES_HXX

#include "bspf.hxx"
#include "Control.hxx"
#include "Event.hxx"

/**
  The standard Atari 2600 pair of paddle controllers.

  @author  Bradford W. Mott
  @version $Id: Paddles.hxx,v 1.15 2009/01/01 18:13:36 stephena Exp $
*/
class Paddles : public Controller
{
  public:
    /**
      Create a new pair of paddle controllers plugged into the specified jack

      @param jack   The jack the controller is plugged into
      @param event  The event object to use for events
      @param system The system using this controller
      @param swap   Whether to swap the paddles plugged into this jack
    */
    Paddles(Jack jack, const Event& event, const System& system, bool swap);

    /**
      Destructor
    */
    virtual ~Paddles();

  public:
    /**
      Update the entire digital and analog pin state according to the
      events currently set.
    */
    virtual void update();

    /**
      Sets the speed for digital emulation of paddle movement.
      This is only used for *digital* events (ie, buttons or keys
      generating paddle movement events); axis events from joysticks,
      Stelladaptors or the mouse are not modified.
    */
    static void setDigitalSpeed(int speed) { _PADDLE_SPEED = speed; }

    /**
      Sets the mouse to emulate the paddle 'number' in the X or Y
      axis.  X -> dir 0, Y -> dir 1
    */
    static void setMouseIsPaddle(int number, int dir = 0);

#ifdef WII
    void setRightPaddleOffset( const int offset );
#endif

  private:
    // Pre-compute the events we care about based on given port
    // This will eliminate test for left or right port in update()
    Event::Type myP0AxisValue, myP1AxisValue,
                myP0DecEvent1, myP0DecEvent2, myP0IncEvent1, myP0IncEvent2,
                myP1DecEvent1, myP1DecEvent2, myP1IncEvent1, myP1IncEvent2,
                myP0FireEvent1, myP0FireEvent2, myP1FireEvent1, myP1FireEvent2;

    int myKeyRepeat0;
    int myPaddleRepeat0;
    int myKeyRepeat1;
    int myPaddleRepeat1;

    int myCharge[2];
    int myLastCharge[2];
    int myLeftMotion[2];

#ifdef WII
    /**
     * Initializes the settings for Wii paddle support
     *
     * swap - Whether we are swapping controllers
     */
    void initWiiSettings( const bool swap );

    /**
     * Returns the position of the paddle for the specified 
     * controller
     *
     * prev - The last position for the controller
     * mote - The index of the controller
     */
    int getWiiPaddlePosition( const int prev, const int mote );    

    /** Are we swapping paddles */
    bool wiiSwap;
    /** The previous paddle position (paddle 0) */
    int wiiPrevp0;
    /** The previous paddle position (paddle 1) */
    int wiiPrevp1;
    /** The paddle range */
    float wiiPaddleRange;

    // Wii - IR mode
    float wiiIrMax;
    float wiiIrMin;
    float wiiIrRange;

    // Wii - Roll mode
    float wiiRollMax;
    float wiiRollMin;
    float wiiRollRange;
    float wiiRollCenter;
    float wiiRollRatio;

    // Wii - analog 
    float wiiAnalogRatio;
    float wiiAnalogMin;
    float wiiAnalogMax;
    float wiiAnalogRange;

    // Gamecube - analog 
    float gcAnalogRatio;
    float gcAnalogMin;
    float gcAnalogMax;
    float gcAnalogRange;

    int wiiRightPaddleOffset;
#endif

    static int _PADDLE_SPEED;
    static int _MOUSEX_PADDLE;
    static int _MOUSEY_PADDLE;

    // Lookup table for associating paddle buttons with controller pins
    // Yes, this is hideously complex
    static const Controller::DigitalPin ourButtonPin[2];
};

#endif

