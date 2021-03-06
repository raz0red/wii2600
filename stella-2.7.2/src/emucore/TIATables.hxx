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
// $Id: TIATables.hxx,v 1.1 2009/01/19 16:52:32 stephena Exp $
//============================================================================

#ifndef TIA_TABLES_HXX
#define TIA_TABLES_HXX

#include "bspf.hxx"

enum {
  P0Bit       = 0x01,  // Bit for Player 0
  M0Bit       = 0x02,  // Bit for Missle 0
  P1Bit       = 0x04,  // Bit for Player 1
  M1Bit       = 0x08,  // Bit for Missle 1
  BLBit       = 0x10,  // Bit for Ball
  PFBit       = 0x20,  // Bit for Playfield
  ScoreBit    = 0x40,  // Bit for Playfield score mode
  PriorityBit = 0x80   // Bit for Playfield priority
};

// TIA Write/Read register names
enum {
  VSYNC   = 0x00,  // Write: vertical sync set-clear (D1)
  VBLANK  = 0x01,  // Write: vertical blank set-clear (D7-6,D1)
  WSYNC   = 0x02,  // Write: wait for leading edge of hrz. blank (strobe)
  RSYNC   = 0x03,  // Write: reset hrz. sync counter (strobe)
  NUSIZ0  = 0x04,  // Write: number-size player-missle 0 (D5-0)
  NUSIZ1  = 0x05,  // Write: number-size player-missle 1 (D5-0)
  COLUP0  = 0x06,  // Write: color-lum player 0 (D7-1)
  COLUP1  = 0x07,  // Write: color-lum player 1 (D7-1)
  COLUPF  = 0x08,  // Write: color-lum playfield (D7-1)
  COLUBK  = 0x09,  // Write: color-lum background (D7-1)
  CTRLPF  = 0x0a,  // Write: cntrl playfield ballsize & coll. (D5-4,D2-0)
  REFP0   = 0x0b,  // Write: reflect player 0 (D3)
  REFP1   = 0x0c,  // Write: reflect player 1 (D3)
  PF0     = 0x0d,  // Write: playfield register byte 0 (D7-4)
  PF1     = 0x0e,  // Write: playfield register byte 1 (D7-0)
  PF2     = 0x0f,  // Write: playfield register byte 2 (D7-0)
  RESP0   = 0x10,  // Write: reset player 0 (strobe)
  RESP1   = 0x11,  // Write: reset player 1 (strobe)
  RESM0   = 0x12,  // Write: reset missle 0 (strobe)
  RESM1   = 0x13,  // Write: reset missle 1 (strobe)
  RESBL   = 0x14,  // Write: reset ball (strobe)
  AUDC0   = 0x15,  // Write: audio control 0 (D3-0)
  AUDC1   = 0x16,  // Write: audio control 1 (D4-0)
  AUDF0   = 0x17,  // Write: audio frequency 0 (D4-0)
  AUDF1   = 0x18,  // Write: audio frequency 1 (D3-0)
  AUDV0   = 0x19,  // Write: audio volume 0 (D3-0)
  AUDV1   = 0x1a,  // Write: audio volume 1 (D3-0)
  GRP0    = 0x1b,  // Write: graphics player 0 (D7-0)
  GRP1    = 0x1c,  // Write: graphics player 1 (D7-0)
  ENAM0   = 0x1d,  // Write: graphics (enable) missle 0 (D1)
  ENAM1   = 0x1e,  // Write: graphics (enable) missle 1 (D1)
  ENABL   = 0x1f,  // Write: graphics (enable) ball (D1)
  HMP0    = 0x20,  // Write: horizontal motion player 0 (D7-4)
  HMP1    = 0x21,  // Write: horizontal motion player 1 (D7-4)
  HMM0    = 0x22,  // Write: horizontal motion missle 0 (D7-4)
  HMM1    = 0x23,  // Write: horizontal motion missle 1 (D7-4)
  HMBL    = 0x24,  // Write: horizontal motion ball (D7-4)
  VDELP0  = 0x25,  // Write: vertical delay player 0 (D0)
  VDELP1  = 0x26,  // Write: vertical delay player 1 (D0)
  VDELBL  = 0x27,  // Write: vertical delay ball (D0)
  RESMP0  = 0x28,  // Write: reset missle 0 to player 0 (D1)
  RESMP1  = 0x29,  // Write: reset missle 1 to player 1 (D1)
  HMOVE   = 0x2a,  // Write: apply horizontal motion (strobe)
  HMCLR   = 0x2b,  // Write: clear horizontal motion registers (strobe)
  CXCLR   = 0x2c,  // Write: clear collision latches (strobe)

  CXM0P   = 0x00,  // Read collision: D7=(M0,P1); D6=(M0,P0)
  CXM1P   = 0x01,  // Read collision: D7=(M1,P0); D6=(M1,P1)
  CXP0FB  = 0x02,  // Read collision: D7=(P0,PF); D6=(P0,BL)
  CXP1FB  = 0x03,  // Read collision: D7=(P1,PF); D6=(P1,BL)
  CXM0FB  = 0x04,  // Read collision: D7=(M0,PF); D6=(M0,BL)
  CXM1FB  = 0x05,  // Read collision: D7=(M1,PF); D6=(M1,BL)
  CXBLPF  = 0x06,  // Read collision: D7=(BL,PF); D6=(unused)
  CXPPMM  = 0x07,  // Read collision: D7=(P0,P1); D6=(M0,M1)
  INPT0   = 0x08,  // Read pot port: D7
  INPT1   = 0x09,  // Read pot port: D7
  INPT2   = 0x0a,  // Read pot port: D7
  INPT3   = 0x0b,  // Read pot port: D7
  INPT4   = 0x0c,  // Read P1 joystick trigger: D7
  INPT5   = 0x0d   // Read P2 joystick trigger: D7
};

/**
  The TIA class uses some static tables that aren't dependent on the actual
  TIA state.  For code organization, it's better to place that functionality
  here.

  @author  Stephen Anthony
  @version $Id: TIATables.hxx,v 1.1 2009/01/19 16:52:32 stephena Exp $
*/
class TIATables
{
  public:
    /**
      Compute all static tables used by the TIA
    */
    static void computeAllTables();

    // Ball mask table (entries are true or false)
    static uInt8 BallMaskTable[4][4][320];

    // Used to set the collision register to the correct value
    static uInt16 CollisionTable[64];

    // A mask table which can be used when an object is disabled
    static uInt8 DisabledMaskTable[640];

    // Indicates the update delay associated with poking at a TIA address
    static const Int16 PokeDelayTable[64];

    // Missle mask table (entries are true or false)
    static uInt8 MissleMaskTable[4][8][4][320];

    // Used to convert value written in a motion register into 
    // its internal representation
    static const Int32 CompleteMotionTable[76][16];

    // Indicates if HMOVE blanks should occur for the corresponding cycle
    static const bool HMOVEBlankEnableCycles[76];

    // Player mask table
    static uInt8 PlayerMaskTable[4][2][8][320];

    // Indicates if player is being reset during delay, display or other times
    static Int8 PlayerPositionResetWhenTable[8][160][160];

    // Used to reflect a players graphics
    static uInt8 PlayerReflectTable[256];

    // Playfield mask table for reflected and non-reflected playfields
    static uInt32 PlayfieldTable[2][160];

  private:
    // Compute the ball mask table
    static void computeBallMaskTable();

    // Compute the collision decode table
    static void computeCollisionTable();

    // Compute the missle mask table
    static void computeMissleMaskTable();

    // Compute the player mask table
    static void computePlayerMaskTable();

    // Compute the player position reset when table
    static void computePlayerPositionResetWhenTable();

    // Compute the player reflect table
    static void computePlayerReflectTable();

    // Compute playfield mask table
    static void computePlayfieldMaskTable();
};

#endif
