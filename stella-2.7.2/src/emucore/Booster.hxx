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
// $Id: Booster.hxx,v 1.12 2009/01/01 18:13:35 stephena Exp $
//============================================================================

#ifndef BOOSTERGRIP_HXX
#define BOOSTERGRIP_HXX

#include "bspf.hxx"
#include "Control.hxx"
#include "Event.hxx"

/**
  The standard Atari 2600 joystick controller fitted with the 
  CBS Booster grip.  The Booster grip has two more fire buttons 
  on it (a booster and a trigger).

  @author  Bradford W. Mott
  @version $Id: Booster.hxx,v 1.12 2009/01/01 18:13:35 stephena Exp $
*/
class BoosterGrip : public Controller
{
  public:
    /**
      Create a new booster grip joystick plugged into the specified jack

      @param jack   The jack the controller is plugged into
      @param event  The event object to use for events
      @param system The system using this controller
    */
    BoosterGrip(Jack jack, const Event& event, const System& system);

    /**
      Destructor
    */
    virtual ~BoosterGrip();

  public:
    /**
      Update the entire digital and analog pin state according to the
      events currently set.
    */
    virtual void update();

  private:
    // Pre-compute the events we care about based on given port
    // This will eliminate test for left or right port in update()
    Event::Type myUpEvent, myDownEvent, myLeftEvent, myRightEvent,
                myFireEvent, myBoosterEvent, myTriggerEvent,
                myXAxisValue, myYAxisValue;
};

#endif
