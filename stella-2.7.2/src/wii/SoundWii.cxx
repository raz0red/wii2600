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
//============================================================================

//#ifdef SOUND_SUPPORT

#include <sstream>
#include <cassert>
#include <cmath>
#include <SDL.h>

#include "TIASnd.hxx"
#include "FrameBuffer.hxx"
#include "Serializer.hxx"
#include "Deserializer.hxx"
#include "Settings.hxx"
#include "System.hxx"
#include "OSystem.hxx"

#include "Console.hxx"
#include "AtariVox.hxx"
#ifdef SPEAKJET_EMULATION
#include "SpeakJet.hxx"
#endif

#include "SoundWii.hxx"

#include "wii_audio.hxx"
#include "wii_app.hxx"
static SDL_AudioCVT audio_convert;

#define SAMPLERATE 32000 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundWii::SoundWii(OSystem* osystem)
: Sound(osystem),
myIsEnabled(osystem->settings().getBool("sound")),
myIsInitializedFlag(false),
myLastRegisterSetCycle(0),
myDisplayFrameRate(60.0),
myNumChannels(1),
myFragmentSizeLogBase2(0),
myIsMuted(true),
myVolume(100)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundWii::~SoundWii()
{
    // Close the SDL audio system if it's initialized
    close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::setEnabled(bool state)
{
    myIsEnabled = state;
    myOSystem->settings().setBool("sound", state);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::open()
{
    // Check whether to start the sound subsystem
    if(!myIsEnabled)
    {
        close();
        if(myOSystem->settings().getBool("showinfo"))
            cout << "Sound disabled." << endl << endl;
        return;
    }

    // Make sure the sound queue is clear
    myRegWriteQueue.clear();
    myTIASound.reset();

    SDL_BuildAudioCVT(
        &audio_convert,
        AUDIO_U8,
        1,
        SAMPLERATE,
        AUDIO_S16MSB, 
        2,
        SAMPLERATE
        );
    
    myLastRegisterSetCycle = 0;
    //myFragmentSizeLogBase2 = log((double)640) / log(2.0); // WII

    // Now initialize the TIASound object which will actually generate sound
    myTIASound.outputFrequency(SAMPLERATE);
    myTIASound.tiaFrequency(SAMPLERATE); //31400
    myTIASound.channels(1);

    bool clipvol = myOSystem->settings().getBool("clipvol");
    myTIASound.clipVolume(clipvol);

    // Adjust volume to that defined in settings
    myVolume = myOSystem->settings().getInt("volume");
    setVolume(myVolume);

    InitialiseAudio();

    myIsInitializedFlag = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::close()
{
    if(myIsInitializedFlag)
    {
        StopAudio();
        myIsInitializedFlag = false;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundWii::isSuccessfullyInitialized() const
{
    return myIsInitializedFlag;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::mute(bool state)
{
    if(myIsInitializedFlag)
    {
        // Ignore multiple calls to do the same thing
        if(myIsMuted == state)
        {
            return;
        }

        myIsMuted = state;

        if( myIsMuted )
        {
            StopAudio();        
        }

        ResetAudio();    
        myRegWriteQueue.clear();
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::reset()
{
    if(myIsInitializedFlag)
    {
        StopAudio();
        myIsMuted = false;
        myLastRegisterSetCycle = 0;
        myTIASound.reset();
        myRegWriteQueue.clear();
        ResetAudio();
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::setVolume(Int32 percent)
{
    if(myIsInitializedFlag)
    {
        if((percent >= 0) && (percent <= 100))
        {
            myOSystem->settings().setInt("volume", percent);
            myVolume = percent;
            myTIASound.volume(percent);
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::adjustVolume(Int8 direction)
{
    ostringstream strval;
    string message;

    Int32 percent = myVolume;

    if(direction == -1)
        percent -= 2;
    else if(direction == 1)
        percent += 2;

    if((percent < 0) || (percent > 100))
        return;

    setVolume(percent);

    // Now show an onscreen message
    strval << percent;
    message = "Volume set to ";
    message += strval.str();

    myOSystem->frameBuffer().showMessage(message);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::adjustCycleCounter(Int32 amount)
{
    myLastRegisterSetCycle += amount;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::setChannels(uInt32 channels)
{
    if(channels == 1 || channels == 2)
        myNumChannels = channels;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::setFrameRate(float framerate)
{
    // FIXME - should we clear out the queue or adjust the values in it?
    myDisplayFrameRate = framerate;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::set(uInt16 addr, uInt8 value, Int32 cycle)
{

    // First, calulate how many seconds would have past since the last
    // register write on a real 2600
    double delta = (((double)(cycle - myLastRegisterSetCycle)) / 
        (1193191.66666667));

    // Now, adjust the time based on the frame rate the user has selected. For
    // the sound to "scale" correctly, we have to know the games real frame 
    // rate (e.g., 50 or 60) and the currently emulated frame rate. We use these
    // values to "scale" the time before the register change occurs.
    // FIXME - this always results in 1.0, so we don't really need it
    //  delta = delta * (myDisplayFrameRate / myOSystem->frameRate());
    RegWrite info;
    info.addr = addr;
    info.value = value;
    info.delta = delta;
    myRegWriteQueue.enqueue(info);

    // Update last cycle counter to the current cycle
    myLastRegisterSetCycle = cycle;

}

static int lastDisplayFrameRate = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::processFragment(uInt8* stream, Int32 length)
{
    if(!myIsInitializedFlag)
        return;

    if( lastDisplayFrameRate != (int)myDisplayFrameRate )
    {
        myFragmentSizeLogBase2 = log((double)(SAMPLERATE/(int)myDisplayFrameRate)) / log(2.0); // WII
        lastDisplayFrameRate = (int)myDisplayFrameRate;
    }

    uInt32 channels = 1 /*myHardwareSpec.channels*/;
    length = length / channels;

    // If there are excessive items on the queue then we'll remove some
    if(myRegWriteQueue.duration() > 
        (myFragmentSizeLogBase2 / myDisplayFrameRate))
    {
        double removed = 0.0;
        while(removed < ((myFragmentSizeLogBase2 - 1) / myDisplayFrameRate))
        {
            RegWrite& info = myRegWriteQueue.front();
            removed += info.delta;
            myTIASound.set(info.addr, info.value);
            myRegWriteQueue.dequeue();
        }
        //    cout << "Removed Items from RegWriteQueue!" << endl;
    }

    double position = 0.0;
    double remaining = length;

    while(remaining > 0.0)
    {
        if(myRegWriteQueue.size() == 0)
        {
            // There are no more pending TIA sound register updates so we'll
            // use the current settings to finish filling the sound fragment
            //    myTIASound.process(stream + (uInt32)position, length - (uInt32)position);
            myTIASound.process(stream + ((uInt32)position * channels),
                length - (uInt32)position);

            // Since we had to fill the fragment we'll reset the cycle counter
            // to zero.  NOTE: This isn't 100% correct, however, it'll do for
            // now.  We should really remember the overrun and remove it from
            // the delta of the next write.
            myLastRegisterSetCycle = 0;
            break;
        }
        else
        {
            // There are pending TIA sound register updates so we need to
            // update the sound buffer to the point of the next register update
            RegWrite& info = myRegWriteQueue.front();

            // How long will the remaining samples in the fragment take to play
            double duration = remaining / (double)SAMPLERATE;

            // Does the register update occur before the end of the fragment?
            if(info.delta <= duration)
            {
                // If the register update time hasn't already passed then
                // process samples upto the point where it should occur
                if(info.delta > 0.0)
                {
                    // Process the fragment upto the next TIA register write.  We
                    // round the count passed to process up if needed.
                    double samples = (SAMPLERATE * info.delta);
                    //        myTIASound.process(stream + (uInt32)position, (uInt32)samples +
                    //            (uInt32)(position + samples) - 
                    //            ((uInt32)position + (uInt32)samples));
                    myTIASound.process(stream + ((uInt32)position * channels),
                        (uInt32)samples + (uInt32)(position + samples) - 
                        ((uInt32)position + (uInt32)samples));

                    position += samples;
                    remaining -= samples;
                }
                myTIASound.set(info.addr, info.value);
                myRegWriteQueue.dequeue();
            }
            else
            {
                // The next register update occurs in the next fragment so finish
                // this fragment with the current TIA settings and reduce the register
                // update delay by the corresponding amount of time
                //      myTIASound.process(stream + (uInt32)position, length - (uInt32)position);
                myTIASound.process(stream + ((uInt32)position * channels),
                    length - (uInt32)position);
                info.delta -= duration;
                break;
            }
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundWii::load(Deserializer& in)
{
    string device = "TIASound";

    try
    {
        if(in.getString() != device)
            return false;

        uInt8 reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0, reg5 = 0, reg6 = 0;
        reg1 = (uInt8) in.getByte();
        reg2 = (uInt8) in.getByte();
        reg3 = (uInt8) in.getByte();
        reg4 = (uInt8) in.getByte();
        reg5 = (uInt8) in.getByte();
        reg6 = (uInt8) in.getByte();

        myLastRegisterSetCycle = (Int32) in.getInt();

        // Only update the TIA sound registers if sound is enabled
        // Make sure to empty the queue of previous sound fragments
        if(myIsInitializedFlag)
        {
            StopAudio();
            myRegWriteQueue.clear();
            myTIASound.set(0x15, reg1);
            myTIASound.set(0x16, reg2);
            myTIASound.set(0x17, reg3);
            myTIASound.set(0x18, reg4);
            myTIASound.set(0x19, reg5);
            myTIASound.set(0x1a, reg6);
            ResetAudio();
        }
    }
    catch(char *msg)
    {
        cerr << msg << endl;
        return false;
    }
    catch(...)
    {
        cerr << "Unknown error in load state for " << device << endl;
        return false;
    }

    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundWii::save(Serializer& out)
{
    string device = "TIASound";

    try
    {
        out.putString(device);

        uInt8 reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0, reg5 = 0, reg6 = 0;

        // Only get the TIA sound registers if sound is enabled
        if(myIsInitializedFlag)
        {
            reg1 = myTIASound.get(0x15);
            reg2 = myTIASound.get(0x16);
            reg3 = myTIASound.get(0x17);
            reg4 = myTIASound.get(0x18);
            reg5 = myTIASound.get(0x19);
            reg6 = myTIASound.get(0x1a);
        }

        out.putByte((char)reg1);
        out.putByte((char)reg2);
        out.putByte((char)reg3);
        out.putByte((char)reg4);
        out.putByte((char)reg5);
        out.putByte((char)reg6);

        out.putInt(myLastRegisterSetCycle);
    }
    catch(char *msg)
    {
        cerr << msg << endl;
        return false;
    }
    catch(...)
    {
        cerr << "Unknown error in save state for " << device << endl;
        return false;
    }

    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundWii::RegWriteQueue::RegWriteQueue(uInt32 capacity)
: myCapacity(capacity),
myBuffer(0),
mySize(0),
myHead(0),
myTail(0)
{
    myBuffer = new RegWrite[myCapacity];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundWii::RegWriteQueue::~RegWriteQueue()
{
    delete[] myBuffer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::RegWriteQueue::clear()
{
    myHead = myTail = mySize = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::RegWriteQueue::dequeue()
{
    if(mySize > 0)
    {
        myHead = (myHead + 1) % myCapacity;
        --mySize;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double SoundWii::RegWriteQueue::duration()
{
    double duration = 0.0;
    for(uInt32 i = 0; i < mySize; ++i)
    {
        duration += myBuffer[(myHead + i) % myCapacity].delta;
    }
    return duration;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::RegWriteQueue::enqueue(const RegWrite& info)
{
    // If an attempt is made to enqueue more than the queue can hold then
    // we'll enlarge the queue's capacity.
    if(mySize == myCapacity)
    {
        grow();
    }

    myBuffer[myTail] = info;
    myTail = (myTail + 1) % myCapacity;
    ++mySize;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundWii::RegWrite& SoundWii::RegWriteQueue::front()
{
    assert(mySize != 0);
    return myBuffer[myHead];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 SoundWii::RegWriteQueue::size() const
{
    return mySize;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundWii::RegWriteQueue::grow()
{
    RegWrite* buffer = new RegWrite[myCapacity * 2];
    for(uInt32 i = 0; i < mySize; ++i)
    {
        buffer[i] = myBuffer[(myHead + i) % myCapacity];
    }
    myHead = 0;
    myTail = mySize;
    myCapacity = myCapacity * 2;
    delete[] myBuffer;
    myBuffer = buffer;
}

static int i = 0;

void SoundWii::update(void)
{
    if( wii_force_fps != -1 )
    {
        myDisplayFrameRate = wii_force_fps;
    }

    if (myIsMuted) return;

    static u8 buffer[4096];
    unsigned int nsamples = (SAMPLERATE / (int)myDisplayFrameRate);
    memset( buffer, 0x80, 4096 );

    processFragment( buffer, nsamples );

    audio_convert.buf = buffer;
    audio_convert.len = nsamples;
    SDL_ConvertAudio( &audio_convert );

#if 0
    if( ( i++ % 100 ) == 0 )
    {   
        fprintf( stderr, "update sound: %d, %d, %d, %d, %d, %d\n", 
            myNumChannels, 
            nsamples, 
            audio_convert.len_mult,
            audio_convert.len_cvt
            );
    }
#endif

    PlaySound( (u32*)buffer, ( audio_convert.len_cvt / 4 ) );        
}

//#endif
