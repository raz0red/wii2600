--------------------------------------------
Wii2600 v0.2 release README
--------------------------------------------

Ported by raz0red
[http://www.twitchasylum.com]

--------------------------------------------
What is Wii2600?
--------------------------------------------

Wii2600 is a port of the Stella 2.7.2 emulator to the Wii.

Stella is a multi-platform Atari 2600 VCS emulator released under the GNU 
General Public License (GPL). Stella was originally developed for Linux by 
Bradford W. Mott, however, since its original release several people have 
joined the development team to port Stella to other operating systems such 
as AcornOS, AmigaOS, DOS, FreeBSD, IRIX, Linux, OS/2, MacOS, Unix, and 
Windows. [http://stella.sourceforge.net].

--------------------------------------------
Current status
--------------------------------------------

Wii2600 is an ongoing work in progress. For the latest project information
please visit the following URL:

[http://www.wiibrew.org/wiki/Wii2600]

--------------------------------------------
Known issues
--------------------------------------------

    - Displaying rom info w/ screenshots makes browsing slow
    - The colors are incorrect for screenshots taken at 1x

--------------------------------------------
Installation
--------------------------------------------

To install Wii2600, simply extract the zip file that this README was 
distributed with directly to your SD card (retain the hierarchical structure
exactly).

Cartridge images should be placed in the roms directory (wii2600/roms).

--------------------------------------------
Tearing, Vertical Sync, PAL/NTSC, etc.
--------------------------------------------

Without the use of Vertical Sync, 2600 games that scroll (Barnstorming, River
Raid, etc.) will exhibit tearing during game play.

The first time Wii2600 is started it attempts to determine whether your Wii is
running in PAL (50hz) or NTSC/PAL60 (60hz) mode. Based on this determination it
sets a default value for Vertical Sync. 

NTSC/PAL60:

If Wii2600 determines you are in NTSC/PAL60 mode (60hz) it enables Vertical
Sync by default. This should work well for both NTSC and PAL games. PAL games
run fine due to the fact that they run at a slower rate than NTSC.

PAL:

If Wii2600 determines you are currently in PAL mode (50hz) it disables Vertical
Sync by default. The reason it disables this setting is to be compatible with
both PAL and NTSC games at 100% speed. Since the Wii console is syncing at 50hz 
it won't be able to maintain the required 60hz for NTSC games and as a result 
you will get garbled sound and slow video.

If you still want to use Vertical Sync, you can enable it via the "Advanced" 
menu in Wii2600. PAL games will run as they were intended, 50hz at 100% speed.
Wii2600 also configures another setting "Force FPS" to "50 FPS" when you enable
Vertical Sync in PAL mode. What this setting does is control the frame rate
that games run at. Essentially what it will do is force all games (NTSC or
PAL) to run at 50 FPS. In doing so it also adjusts audio buffer sizes, etc. so
that you won't experience garbled sound. The downside is that NTSC games will
now run at 50 FPS versus their intended 60 FPS, so the games will run a bit 
slower.

The real answer to resolve these issues is to add auto-frame skipping to 
Stella. That is definitely something I will be looking to add in an upcoming
release. 

--------------------------------------------
Paddle controls
--------------------------------------------

Wii2600 supports three different modes for emulating paddles support ("Roll",
"IR", and "Analog"). For complete details see the controls section below. 

In most cases Stella will automatically enable paddle support for the selected
rom. However, in cases where it doesn't, you must configure paddle support 
manually. This is accomplished via the "Controller" tab found in the "Game 
Properties" menu of the "Options" (found in the main rom browser). For the P0 
or P1 controllers specify Paddles instead of Joystick, etc. 

It is also important to note how Wii2600 allocates the controls when Paddles 
are selected. If P0 is set to be paddles, then Wii controllers 1 and 2 will map
to the two paddles that are that are plugged into the virtual P0. If P1 is 
also set to paddles then controllers 3 and 4 will represent the two paddles 
that are plugged into P1. However if P0 is set to be paddles and P1 is set to
be a joystick, then Wii controller 3 will represent the joystick (1 and 2 will
represent paddles). If P1 is set to joystick and P2 is set to paddles then Wii
controller 1 will be the joystick while 2 and 3 will represent the paddles.

--------------------------------------------
Controls
--------------------------------------------

    Wii2600 menu:
    -------------

        Wiimote:

            Up/Down  : Scroll
            A        : Select 
            B        : Back
            Home     : Exit to Homebrew Channel
            Power    : Power off

         Classic controller:

            Up/Down  : Scroll
            A        : Select 
            B        : Back
            Home     : Exit to Homebrew Channel
            
         Nunchuk controller:

            Up/Down  : Scroll
            C        : Select 
            Z        : Back
                  
         GameCube controller:

            Up/Down  : Scroll
            A        : Select 
            B        : Back
            Z        : Exit to Homebrew Channel
            
    Stella emulator menu:
    ---------------------
    
    Unfortunately, at this point all navigation of the Stella menu is done via
    the Wiimote without the use of motion controls. Thus, the '+' and '-' 
    buttons allow for tabbing through the different controls. Pressing the 'A'
    button operates the current control, etc.
    
        Wiimote:

            +           : Move forward through controls
            -           : Move backward through controls
            Up/Down     : Scroll (when focused on a list or slider, etc.)
            Left/Right  : Page through list (when focused on list control)
            Left/Right  : Move between tabs (when focused on a tab in a dialog)
            A           : Operates current control (presses button, launch rom)
            B           : Closes current dialog
            Home        : Exit to Homebrew Channel

         Classic controller:

            +           : Move forward through controls
            -           : Move backward through controls
            R Trigger   : (Alternate) Move forward through controls
            L Trigger   : (Alternate) Move backward through controls
            Up/Down     : Scroll (when focused on a list or slider, etc.)
            Left/Right  : Page through list (when focused on list control)
            Left/Right  : Move between tabs (when focused on a tab in a dialog)
            A           : Operates current control (presses button, launch rom)
            B           : Closes current dialog
            Home        : Exit to Homebrew Channel
            
         Nunchuk controller:

            Up/Down     : Scroll (when focused on a list or slider, etc.)
            Left/Right  : Page through list (when focused on list control)
            Left/Right  : Move between tabs (when focused on a tab in a dialog)
            C           : Operates current control (presses button, launch rom)
            Z           : Closes current dialog
                  
         GameCube controller:

            R Trigger   : Move forward through controls
            L Trigger   : Move backward through controls
            Up/Down     : Scroll (when focused on a list or slider, etc.)
            Left/Right  : Page through list (when focused on list control)
            Left/Right  : Move between tabs (when focused on a tab in a dialog)
            A           : Operates current control (presses button, launch rom)
            B           : Closes current dialog
            Z           : Exit to Homebrew Channel
            
         USB keyboard:
         
            Refer to Stella emulator documentation.             
            
    In-game (Joystick/Driving):
    ---------------------------

        Wiimote:

            D-pad         : Move
            2             : Fire
            1             : Open/close command menu (save/load state, etc.)
            +             : [Reset]
            -             : [Select]
            A             : [Toggle Color/BW mode]
            Home          : Display Stella menu (see above)

         Classic controller:

            D-pad/Analog  : Move
            A             : Fire
            Y             : Open/close command menu (save/load state, etc.)
            +             : [Reset]
            -             : [Select]
            R Trigger     : [Toggle Color/BW mode]
            Home          : Display Stella menu (see above)
            
         Nunchuk controller:

            Analog        : Move
            C/Z           : Fire            
                  
         GameCube controller:

            D-pad/Analog  : Move
            A             : Fire
            Y             : Open/close command menu (save/load state, etc.)
            Start         : [Reset]
            L Trigger     : [Select]
            R Trigger     : [Toggle Color/BW mode]
            Z             : Display Stella menu (see above)
           
         USB keyboard:
         
            Refer to Stella emulator documentation.             
            
    In-game (Paddle - "Roll" mode):
    -------------------------------
    
    In this paddle mode, you hold the Wiimote horizontally and twist it forward
    and backward (like the Wiiware game Bit Trip Beat). You can set the 
    sensitivity and center of this mode via the "Controls" menu. You should 
    definitely adjust the settings until you find one that is comfortable. For 
    example, I like to adjust the center way forward (+5) because I find it 
    easier to push the fire button for games like Warlords, etc. Also, I 
    adjust the sensitivity up to about 7. Since the Wiimote doesn't have to be
    pointed directly at the sensor bar for this mode to work, it works wll on
    all styles of paddle-based games (Kaboom!, Warlords, Video Olympics, etc.).
        
        Wiimote:

            Roll    : Move 
            A/B/2   : Fire
            1       : Open/close command menu (save/load state, etc.)
            +       : [Reset]
            -       : [Select]
            Home    : Display Stella menu (see above)    

                
    In-game (Paddle - "IR" mode):
    -----------------------------
    
    In this paddle mode, you point the Wiimote at the Wii sensor bar and move 
    it left and right. You can set the sensitivity of this mode via the 
    "Controls" menu. This mode requires that the Wiimote be pointed directly
    at the screen, thus it is typically better suited for horizontally oriented
    single-player style games (Kaboom!, Breakout, etc.).
        
        Wiimote:

            IR      : Move 
            A/B/2   : Fire
            1       : Open/close command menu (save/load state, etc.)
            +       : [Reset]
            -       : [Select]
            Home    : Display Stella menu (see above)    
            
            
    In-game (Paddle - Analog joystick mode):
    ----------------------------------------
    
    In this paddle mode, move the Analog joystick left and right (or up and 
    down, see controls menu) to control the paddle. It is important to note 
    that the joystick selected to use for this mode is based on priority. If 
    you have a Nunchuk or Classic controller plugged into the Wiimote, that 
    Analog control will be used. If neither of those are currently plugged in,
    the Gamecube analog control will be used.
        
         Classic controller:

            Analog        : Move
            A             : Fire
            Y             : Open/close command menu (save/load state, etc.)
            +             : [Reset]
            -             : [Select]
            Home          : Display Stella menu (see above)
            
         Nunchuk controller:

            Analog        : Move
            C/Z           : Fire
            
        Wiimote (used w/ Nunchuk):

            A/B/2         : Fire
            1             : Open/close command menu (save/load state, etc.)
            +             : [Reset]
            -             : [Select]
            Home          : Display Stella menu (see above)
                  
         GameCube controller:

            Analog        : Move
            A             : Fire
            Y             : Open/close command menu (save/load state, etc.)
            Start         : [Reset]
            L Trigger     : [Select]
            Z             : Display Stella menu (see above)

--------------------------------------------
Wii2600 crashes, code dumps, etc.
--------------------------------------------

If you are having issues with Wii2600, please let me know about it via one of 
the following locations:

[http://www.wiibrew.org/wiki/Talk:Wii2600]
[http://www.twitchasylum.com/forum/viewtopic.php?t=513]

--------------------------------------------
Special thanks
--------------------------------------------

* munky6           : Lead tester
* RB22             : PAL testing (0.1)
* zodiacprime      : Testing (0.1)
* Sputicus         : Helping bug fix (0.1)
* Tantric/eke-eke  : Audio code example
* yohanes          : SDL USB keyboard support
* Team Twiizers    : For enabling homebrew

--------------------------------------------
Change log
--------------------------------------------

05/03/09 (0.2)
-------------------
    - 4-player paddle support
    - Ability to set the paddle mode to use (Roll, IR, or Analog joystick)    
    - Wiimote "Roll" paddle mode (similar to Bit Trip Beat)
    - Ability to set "Roll" paddle mode sensitivity and center
    - Wiimote "IR" paddle mode (rewritten since 0.1) 
    - Ability to set "IR" paddle mode sensitivity
    - Analog joystick horiz/vert paddle mode (Classic, Nunchuk, Gamecube) 
    - Ability to set analog paddle mode sensitivity
    - Analog joystick control support (in game and during menu navigation) 
    - Driving control support (via d-pads and analog controls)     
    - Ability to toggle color/BW modes from controller (Starmaster, etc.)
    - Improved sound support
    - Fixed bug that caused some games to always display at 1x (versus 2x)
    - Fixed emulation of select/reset buttons (you can now hold them down)
    - Added ability to display rom info (screenshots) when browsing (slow)

02/19/09 (0.1)
--------------
    - Initial release
    - Wiimote/Classic/GameCube controller support
    - Mouse via Wiimote for paddle games (proof of concept)
    - USB Keyboard
    - Audio/Video etc.    
    - Save/load state
    - Screenshot support
