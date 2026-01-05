---
title: "MAMEHOOKER for Light Guns ‐ The Secret Files"
source: "https://github.com/SeongGino/ir-light-gun-plus/wiki/MAMEHOOKER-for-Light-Guns-%E2%80%90-The-Secret-Files"
author:
  - "[[GitHub]]"
published:
created: 2026-01-05
description: "Arduino powered IR light gun - with force-feedback additions, MAMEHOOKER support, quality of life changes, and (possibly) dubious code quality! - MAMEHOOKER for Light Guns ‐ The Secret Files · SeongGino/ir-light-gun-plus Wiki"
tags:
  - "clippings"
---
This repository was archived by the owner on Jun 23, 2024. It is now read-only.

- [MAMEHOOKER Setup Guide](https://github.com/SeongGino/ir-light-gun-plus/wiki/#mamehooker-setup-guide)
- [How does MAMEHOOKER work?](https://github.com/SeongGino/ir-light-gun-plus/wiki/#how-does-mamehooker-work)
- [How does configuration files work?](https://github.com/SeongGino/ir-light-gun-plus/wiki/#how-does-configuration-files-work)
- [Opening, Writing to, and Closing COM Ports](https://github.com/SeongGino/ir-light-gun-plus/wiki/#opening-writing-to-and-closing-com-ports)
- [Light Gun Serial Commands](https://github.com/SeongGino/ir-light-gun-plus/wiki/#light-gun-serial-commands)
- [MAMEHOOKER's Pipe & State Modifiers](https://github.com/SeongGino/ir-light-gun-plus/wiki/#mamehookers-pipe--state-modifiers)

[MAMEHOOKER](http://dragonking.arcadecontrols.com/static.php?page=aboutmamehooker) is... *weird,* as the force feedback capabilities are all controlled from ini files (herein referred to as "configuration files") in `MAMEHOOKER/ini/MAME`. The only documentation concerning creating these files for lightgun games (for systems compatible with it, like GUN4IR or GUN4ALL here) is a small mention of COM devices and that's it. As much as I've tried to find existing configuration files for mamehook, it has ended either in [hostile secrecy from the GUN4IR community](https://mstdn.games/@ThatOneSeong/111382116040036315), or files that *don't even work* or aren't even connected to the right device.

SO, for light gun enthusiasts, here's how this thing works.

### Setup:

- On Windows, MAMEHOOKER requires running with Administrative privileges - otherwise, you'll have an abstract error regarding a DLL file.
- On Linux, MAMEHOOKER requires a wineprefix with the `vbrun6` (Visual Basic 6 Runtime) winetricks verb. This will only work with games running *in the same prefix* and, obviously, only works with Windows games run through Wine (afaik, there is no such outputs support in any native Linux program or emulator... for the two programs that would use it, MAME & RetroArch, anyways).

To have MAMEHOOKER "communicate" with the gun, you'll have to make sure the Arduino's COM port is mapped relative to its intended player number - so if your gun is intended to be Player 1, it has to be COM1, Player 2 is COM2, etc. Mamehook will only see up to COM4, and it's expected that your gun is dedicated to the appropriate slot.

- **To set a COM port on Windows:**
	- While the device is plugged in, go to Device Manager and under the *"Ports (COM & LPT)"* device category, right click on the one that's likely to be your gun (for me, it mapped by default to COM5/6), and click on **Properties** ![Win1](https://camo.githubusercontent.com/76189aa15d9044b3357d55c9a59fa99a3fac47ef75308767ffc499611e41370b/68747470733a2f2f7075752e73682f4a555766682f313534336430623266652e706e67)
	- Navigate to the **Port Settings** tab on the window that pops up, and click on **Advanced...**![Win2](https://camo.githubusercontent.com/15ed483024b8321dce6bbe1d353dbf1f36a92d5b04a071e42de1b7b4f3fd04b3/68747470733a2f2f7075752e73682f4a555766712f373364653538663638312e706e67)
	- Finally, click on the **COM Port Number** dropdown list, and set it to the player port you'd like your gun to be.![Win3](https://camo.githubusercontent.com/02fe43d2f64d8b0c3ac93d5ec133935d1f2327c907ab8a9aa7b764c0077e8b50/68747470733a2f2f7075752e73682f4a555766752f323263396464663936632e706e67)
	- When finished, click **OK** on the Advanced settings window, and **OK** on the driver properties window.
- **To set a COM port on Linux:**
	- The Wineprefix that Mamehook's running in just needs to have these assigned registry keys in the Wine Registry, in `HKEY_LOCAL_MACHINE\Software\Wine\Ports` - where `ttyACM#` is the serial device of the Arduino, and `ttyS0` is built into Linux by default (and normally takes the place of COM1, hence the need to redefine them here).![WineReg](https://camo.githubusercontent.com/eaace690bfe889a69993c713b26edcc8b89f3e940df7799551e7d6c47af59f51/68747470733a2f2f7075752e73682f4a555769452f313333663838623965642e706e67)

If you're using this with outputs provided by DemulShooter (make sure *Outputs* are enabled in `DemulShooter_GUI.exe`), Mamehook *needs to start and have its icon in the system tray first* before DS starts, followed by the game.

---

Games to be hooked into by MAMEHOOKER have an.ini file named after their MAME rom name - or failing that, the shorthand name *of the `-rom`* used by DemulShooter for that game. So if you're configuring for *Terminator 2 - Judgement Day* in MAME, the file used will be `term2.ini` in the `MAMEHOOKER/ini/MAME` directory - likewise, if you're configuring for *Sega Golden Gun* being run in JConfig w/ DemulShooter, the file used will be `sgg.ini`, to match the DS commandline `demulshooter.exe -target=ringwide -rom=sgg`.

- MAME provides outputs natively, and are what MAMEHOOKER routes to PC devices, but it has to be manually enabled by changing the following setting in `mame.ini` to:

```
#
# OSD OUTPUT OPTIONS
#
output                    windows
```

- DemulShooter is needed to provide outputs for PC/Windows-based arcade games and other compatible things. Games in DemulShooter will usually provide a `PX_CtmRecoil` & `PX_Damaged` (PX = player number, i.e. P1 or P2); the former intended for solenoid, the latter intended for rumble.
- Supermodel *technically* provides outputs natively, but it only provides lamp control and no other significant peripheral functionality.

This is the basic anatomy of a MAMEHOOKER-compatible ini file - games can have either all of or less outputs available, and depends on the game whether they provide that output channel or not, but these are what you'll want to focus on mainly:

```
[General]
MameStart=
MameStop=
StateChange=
OnRotate=
OnPause=
[Output]
P1_LmpStart=
P2_LmpStart=
P1_LmpPanel=
P2_LmpPanel=
P1_GunRecoil=
P2_GunRecoil=
P1_GunMotor=
P2_GunMotor=
P1_Ammo=
P2_Ammo=
P1_Clip=
P2_Clip=
P1_CtmRecoil=
P2_CtmRecoil=
P1_Life=
P2_Life=
P1_Damaged=
P2_Damaged=
```

## \[General\]

We only care about two pieces here:

### MameStart

The commands sent when the program is initially hooked and the game is started, and where we establish com ports used, open them, and send the codes to signal that the gun is in serial handoff mode - in that order.

### MameStop

The commands sent when the program is closed, where the signals are sent to end serial communication with the gun and closes the COM port - in that order.

The others in this section don't matter, for Lightguns at least.

## \[Output\]

The meat and potatoes of the configuration, this is where we associate commands with the output 'channels' the game uses here.

These correlate to the lamps/LEDs of the respective Start Buttons or the Panel of the original arcade cabinet corresponding to the player side, and the marquee lighting. For guns, you'll probably like to set the *LmpStart* for the LEDs.

### PX\_GunRecoil

Correlates to the respective player's gun's **recoil feedback,** (usually) in the form of a solenoid, *provided natively by the game.* You'll want to use these for MAME, but skip this for DemulShooter as they might not have the effect you'll anticipate (*Operation G.H.O.S.T.* being an example where this doesn't work well).

### PX\_GunMotor

Correlates to the respective player's gun's **rumble motor,***provided natively by the game.* This can either be the primary form of force feedback in some games (like *Kidou Senshi Gundam: Spirits of Zeon*), or used more conventionally as a subtle feedback during certain game events (as is the case in the *Haunted Museum* games).

### PX\_CtmRecoil

Correlates to the respective player's gun's ***custom solenoid feedback***, *provided by DemulShooter.* This is the feedback channel for solenoids you'll want to be using if you're playing a native Windows game/arcade dump.

### PX\_Damaged

Correlates to the respective player's gun's **vibration feedback when they're damaged,***provided by DemulShooter.* If your game uses `GunMotor` already, you'll probably want to pick either one or the other, or else you won't be able to tell the difference!

### PX\_Ammo/Clip/Life

The number of the respective player's status. I believe these are used for seven-segment displays primarily, but *Aliens Extermination* used it originally for the Pulse Rifle's ammo counter. This is for extra peripherals of the arcade cabinet, not used by GUN4ALL.

Now that you know what the outputs are connected to, how do we use them?

For MAMEHOOKER, any output we want will be sent to a COM port, and doing that is pretty simple: every command needs to start with either a COM port **OPEN** (`cmo #`), **WRITE** (`cmw #`), or **CLOSE** (`cmc #`) - where # is the port that it's directed to. Commands need to be done one at a time, with `,` (comma) telling MAMEHOOKER to do these in succession.

COM port **OPEN** commands start with `cmo`, followed by the port number, then followed by a string that initializes the com port environment. For simplicity sake,

```
cmo 1 baud=9600_parity=N_data=8_stop=1, cmo 2 baud=9600_parity=N_data=8_stop=1,
```

...is what you'll be using in `MameStart` to open COM1 and COM2. But this won't do anything on its own - we need to actually be able to send commands.

COM port **WRITE** commands start with `cmw`, followed by the port number, then followed by the "message" you'll be sending for the gun to process. The gun needs to be signaled that the PC wants to let it hand control over, so after the `cmo` COM open commands, we write:

```
cmw 1 S, cmw 2 S,
```

...this sends a basic start command to the gun, which signals it to enter serial handoff mode. We do the same, only with a different command, for the `MameStop` line for when we're *ending* communication...

COM port **CLOSE** commands, therefore, does exactly that; starting with `cmc`, followed by the port number, this closes the communication channel for the port. So for `MameStop`, we'll instead use:

```
cmw 1 E, cmw 2 E, cmc 1, cmc 2
```

...this sends the serial END command to the gun, telling it that the PC is done handling force feedback and relents control over FF back to the gun's own routines, and then properly closes the channel thereafter.

The only other gun system that has MAMEHOOKER compatibility, *GUN4IR,* has seemingly established a common 'language', or a standardized set of shorthand strings that the gun looks for and responds to. *IR-GUN4ALL* does its best to replicate this by implementing the most important and relevant bits that we can use; for documentation purposes, the entire 'language' will be inscribed here, with GUN4ALL-specific notes where needed.

Does what it says on the tin, *starts* the serial mode on the gun - GUN4ALL will detect this, flip the `serialMode` switch on, and light the board's builtin &/or external LED (if any) with a mid-intensity gray to indicate this switchover. The START command technically has "modes" that it defines:

- `S0` - Start with solenoid enabled
- `S1` - Start with rumble enabled
- `S2` - Start with the RED LED enabled
- `S3` - Start with the GREEN LED enabled
- `S4` - Start with the BLUE LED enabled
- `S6` - Start with everything enabled

...but it seems more like a *suggestion* than anything else as, again, it's the configuration's job to determine what devices it uses. The only thing GUN4ALL looks for is the `S`; everything else is irrelevant.

Again, does what it says on the tin; signals the gun to exit serial mode and go back to handling its force feedback internally. Any force feedback ongoing will get shut off, and the LED will likewise turn off. There's only one kind of END command:

- `E` - Ends serial communication

These set different parameters on the gun to tell it how it should behave in the game. The different "modes" are:

- `M1` - Offscreen Firing Mode
	- `x0` - Disabled
	- `x1` - Fire in bottom-left corner
	- `x2` - Offscreen Button Mode enabled (i.e. offscreen trigger pulls generates a Right Click instead of a Left Click)
	- `x3` - "True Offscreen Firing mode" enabled (i have no idea what this means)
- `M3` - Aspect Ratio correction
	- `x0` - Fullscreen
	- `x1` - 4:3 mode
- `M5` - Auto Reload
	- `x0` - Disabled
	- `x1` - Enabled
- `M8` - Autofire Mode
	- `x0` - Disabled
	- `x1` - Auto fire on
	- `x2` - Auto fire always on

Currently (as of *v3.0 - Rinko*), GUN4ALL detects `M1x2` to automatically toggle offscreen button shooting on for the game (both outside of and in serial handoff mode) and `M8xY` for toggling the gun's burst firing for `x1` and full auto fire for `x2` when outside of serial mode. `M3` and `M5` setting types are not implemented for various reasons.

These are the commands that will directly activate/deactivate the gun's peripherals. They are as follows:

- `F0` - Solenoid
	- `x0` - Off
	- `x1` - On
	- `x2` - Pulse
		- `x#` - Number of pulses, from 0 (none) to 255 (too many)
- `F1` - Rumble
	- `x0` - Off
	- `x1` - On
	- `x2` - Pulse
		- `x#` - Number of pulses, from 0 (none) to 255 (too many)
- `F2` - RGB Red Channel
- `F3` - RGB Green Channel
- `F4` - RGB Blue Channel
	- `x0` - Off
	- `x1` - On
		- `x#` - Strength of color, from 0 (off) to 255 (full intensity)
	- `x2` - Pulse
		- `x#` - Number of pulses, from 0 (none) to 255 (too many)

The peripheral on/off signals are pretty straightforward, but the pulses bit is the amount that the gun itself is supposed to "pulse" the specific peripheral for on its own - or an automated ramping/descending action of each bit, where appropriate. GUN4ALL supports all of these commands, though the LED emitter, in this case, currently, will be reflected on the board's builtin DotStar/NeoPixel, if available.

These commands can be combined - so if you want to have a command both pull the solenoid and start the motor, you'll make that command:

```
cmw # F0x1xF1x1
```

However, these are just for set states - many of these games have different states that each output channel can be in. This is where you'll want to use...

For example, you want the `P1_CtmRecoil` to flip the solenoid on and off, depending on if a shot is happening. It's a very straightforward output, but it has two states - *on* and *off*. MAMEHOOKER takes care of both *engaging* and *disabling* peripherals, but there's only one line for each channel, right?

This is where the `%s%` modifier comes in handy; when used, MAMEHOOKER will automatically substitute `%s%` for the on/off bit of that output channel. So our basic solenoid channel would look something like:

```
cmw 1 F0x%s%
```

This will essentially act as *both* `F0x0` (solenoid disable) *and* `F0x1` (solenoid enable), determined by the state it should be in as determined by the game.

That's great and all for the FF, but what about the LEDs? What if you want them to *pulse*? Well, a game doesn't normally provide a `2` state bit (as far as I'm aware), but we *can* substitute what the "on" and "off" functions do without necessarily having to match the state bit sent by the game!

This is why `|` exists; when used, it will indicate to MAMEHOOKER that these are different commands used for different states of this output channel, and we can individually define what these states do. So if you want the green LED to only pulse when the start button lamp is set on, you would do:

```
cmw 1 F3x0|cmw 1 F3x2x1
```

The left side in this case corresponds to "off", the right side to "on". So when the Start button lamp would turn on, it sends a pulse signal to the green RGB emitter channel - otherwise, sends a green RGB off command. Or, if you want the LED to be used to correspond to the player's health, like for DemulShooter's `P1_Health` output channel, you would do:

```
cmw 1 F2x1x255|cmw 1 F2x1x150|cmw 1 F2x1x50|cmw 1 F3x1x100|cmw 1 F3x1x255
```

This means, from left to right, set the Red LED all the way on, semi-on, dimly lit, then the Green LED goes semi-on, then all the way on to correspond to the player having 1 to 5 hitpoints left in that order.

How do you find this out? Well, that's where the game-specific research comes in. If you don't have a pre-fabbed configuration file for the game you want to have outputs, you'll have to make it yourself; MAMEHOOKER's main window, also known as the Debug Window, will display the game's outputs (if any available/detected) and their states as the game is being played. So you'll have to play the game through (preferably in windowed mode, with the MAMEHOOK debug window visible), note the output channels, and what states they can be in, and then configure the ones you want to make use of for each possible state they can be in.

Yeah, *that takes work,* and you can probably guess why these aren't in great abundance.

If you, or someone you know, has output files for GUN4IR that you'd like to share, I would appreciate the contributions to add to an open-source repository for all gun systems (for GUN4ALL, as well as whenever Sinden adds support) that's freely available to the public.