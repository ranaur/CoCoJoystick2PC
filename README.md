# CoCoJoystick2PC

Make a CoCo (Color Computer) Joystick a USB device (and use in your emulator) with an Arduino Leonardo!

The idea is to use a simple and cheap Arduino Leonardo to simulate a HID Gamepad that reads from a original Color Computer Joystick.

## Bill Of Materials

The Bill Of Materials should be minimal:

* An Arduino Leonardo or Compatible
* A DIN-6 female connector to insert the joystick - or two, if you want to handle two joysticks.
* A push button (to set the calibrate mode)
* Optional: pin switches (or you can wire directly if you don't want to configure the USB behavior)

If you buy an original Leonardo, it would cost around US$ 25. But it will be much cheaper (like less than half) if you buy a compatible board with ATMEGA32U4 controller.

## Subprojects

There are some subprojects (directories with .INO files) I did to test some features. They are not part of the main project, and are here just for educational (and backup) purposes:

* CoCoDetectJoystick: detects when the Joystick is inserted and removed
* Prototype: just a first prototype to test the circuit. Not Usable.
* Prototype2: a second prototype, a working one. The debouncing is simple (just a delay(100)), has calibration, but the code is a mess. It works, and the main ideas of the project are already there.
* CoCoJoystick2USB: the main project, this one is the real one. (not yet in the repository, sorry).

Since version 0.3 the project don't use HID-Project (2.8.2) anymore (but I'm grateful for the insights and idea for implementing the HID protocol!). To add it go in the Arduino IDE menu: Tools/Manage Libraries ... Search for HID-Project and install it.

## Files on Main project

Since the second prototype, things got a little bit more complex and organized. Lets describe the main classes/modules:

CoCoJoystick2USB.ino => main project file. Quite simple, because it instantiates all the classes that handles everything that is needed. The real job is done by CoCoJoystick class.

CoCoJoystick.cpp/.h => handles one Joystick. The setup() method defines which pins will be used for reading the joystick info (X Axis, Y Axis, Button 1 and Button 2) and the offset of the calibration data. The loop() method handle the reading of the pins and the delegation to a handler function. It also handles the calibration routines (startCalibration, centerCalibration and endCalibration) and loading/saving it to EEPROM. The calibration is done in two phases. First phase it read the maximum/minimum of the analog joystick (the user should move the stick as far as he can in all directions). The second phase tests for the center. The user cam move it slightly to handle any vibration of the stick. Or leave it in the center. If the stick goes back to any place of this region it will be considered as "center". The calibration data is saved on the EEPROM in a offset address.
	The reading of values are done by two classes. The stick (analog) is read by two AxisEvent instances (one for X and one for Y). The buttons are handled by two instances of ButtonEvent class.

\*Event Classes => While they are different classes, both share the same paradigm. Run setup() to configure parameters (i.e. pin number, tolecance, EEPROM Offset of calibration data ...) on the loop() routine of the microprocessor call the on\*() classes, that will handle reading, and, if necessary, call the first parameter, that is a callback function. Also, the on\*() methods can receive one pointer to pass to the callback function (Normally the event object itself, but it ca be anything), and the timer (optional). The timer parameter exist to call mills() just once and reuse the value through many calls in the loop() routine. Best use example is CoCoJoystick::loop().

AxisEvent.cpp/.h => Handles on axis. Reads the pin (0 to 1023) and normalize taking account of the calibration values. Handles the calibration state, aswell. The method onChanged() fires the callback if the value read from the sensor differs for more than the "tolerance" parameter. It handles the calibration routine aswell.

ButtonEvent.cpp/.h => Handles one button, with debounce. Has methods onPressed, onReleased, onPressedFor, onReleasedFor that calls the callback function on a button press (KEYDOWN), on a button release (KEYUP), after forMs miliseconds of the button press, and the same for the releade

Calibration.cpp/.h => the Calibration Objects encapsulates the real access to EEPROM, and the mapping from real values to adjusted (calibrated) values.

CoCoGamepadConfig.h => Abstract class that represents the interface of the events that can happen with a CoCo Gamepad could do. (press and release of the buttons, axis change).

# Usage

## Calibrate button
If using a spare button for CALIBRATION (#define CALIBRATION 2)
* If pressed, start calibration process. See "Calibration Process".
* If pressed for 10 seconds, reset the calibration to defaults.

If not using a spare button for CALIBRATION (#define CALIBRATION 1)
During the first 60 seconds after inserting the USB connector, if the user press the red button for 5 seconds, the calibration process starts.
	While it can be calibrated, the builtin led blinks continuously.
	When the calibration starts, it blinks once per second and gather the extremes.
	When you press the red button again, it blinks twice per second telling it is gathering the center region.
	When you press the red button, times out, it finishes the calibration.
	When the 60 seconds passes, it turns the led off.

## Calibration process

* Center Joystick and press the calibrate button
* Move the joystick to extremes: left right, up and down. Circle it a couple of times. Center it again.
* Press the red button
* Leave the joystick at the center or move very slightly (move the base of the joystick, not the stick), just to make the stick oscillate the center.
* Press the red button again
* At any moment pressing the black button cancel the calibration.
* If you stay more than 60 seconds and do not press the red button, the calibration cancels (timeout).

## Useful tips

### Get rid of: "note: #pragma message: Using default ASCII layout for keyboard modules" when compiling

This message is useful if you are programming a keyboard. But we are not. It is just annoying. To get rig of it:

Comment line 54 of ...\arduino\libraries\hid-project\src\keyboardlayouts\improvedkeylayouts.h

	#pragma message "Using default ASCII layout for keyboard modules"

to

	//#pragma message "Using default ASCII layout for keyboard modules"

# Tags

Git tags for the project

v0.1 -	First working version, one joystick, calibration.
v0.2 -	Fix: Now it appears as only one joystick.
	Less debug info.
	Detection Routines added. (needs extra wire shell of the on connector)
v0.3 - Now the HID report only what the joystick really have (2 axis and two buttons)
	Major refactor on calibration
v0.3.1 - More refactor on calibration. Timeout in 10 sec. Button now resets calibration after 3 secs. Black button cancels calibration.
v0.4 - General cleanup, speedup, optimizations, specially on calibration.
v0.5 - First version that handles two joysticks with one board. (if you don't want it, comment #define TWO_JOYSTICK in CoCoJoystick2PC.ino)
v0.6 - Enhancements when handling two joysticks. Now calibration start if the user press the button for 5 seconds in the first 60 seconds after inserting the device.
v0.6-BEETLE - version suitable to use in an Beetle Leonardo. BOM is only the [beetle board](https://www.aliexpress.com/item/1005003830541782.html), a DIN-6 Female Connector and wire to connect them. 

# TODO

## Next Steps

* Do/Test Single report USB

* Test it on ATTINY85
	- Use LUFA to handle USB?
* Automatic firing
	- use potentiometer for time
	- calibrate timing

## Waiting for the new board that works on breadboard
* Add a repeater on buttons
	- Add a potentiometer for speed (0-1023 ms) (>1000 => NO REPEAT)
	- Do it on buttons due lack of analog input: press button for repeat "save". Test average/minimum between presses. Long press cancel.

* Create config switches:
	* Red/Black reports as both buttons (for joysticks that has no secondary button) (3 secs press or when ping 6 is not connected)
	* Swap vertical axis (CP-400 mode)

* Create Switches: (A)
	- class SwitchEvent(nBits) {
		onChange()
		value()
	}

## Two Joysticks, one box
* Allow more than one joystick
	* with 2 joysticks report as two devices
		* On Gamepad Mode, report as one device, left Joy to the left axis control, right joy to right axis control, four buttons.
	* Make it Double: answer as two joysticks if just one joystick is connected
		(if detection is ok, answer always as two: if only one is connected, set the output to both.)

## Test Detection
	>> need to solder cables
	* Connect reset to output with diode and make a programable "reset"
	* detect which joysticks are present on setup and setup usb accordingly
	* when a joystick is connected/disconnectes, resets to redo the report
	* When just one joystick is connected use the single joystick as two joysticks
## Cosmetic changes
* Change the Joystick name from "Arduino Leonardo" to "CoCo Joystick(s)"
	- need to burn a new bootloader?
	- if so, disable "Leonardo" Serial Port

## Make a board to solder

## For future/next project
* Make it Bluetooth
* Make it XBox/Switch Control Compatible mode on Gamepad Mode?

## Pins Planning

 1 - *TX (not used)
 2 - *RX (not used)
 3 - Select (Joy 1)
 4 - Start (Joy 1)
 5 - Command Button (when pressed with joystick buttons ... thing happens)
		Red => Invert buttons, make both (or when does not detect the second button), 
		Black => Invert joysticks
		Y Max / Min => Swap Vertical
		select => Change mode (CoCo/NES/Atari/Gamepad)
		start =>
 6 - Joystick Detection (2nd)
 7 - Black Button (2nd)
 8 - Red Button (2nd)
 9 - Calibrate
10 - Joystick Detection
11 - Black Button
12 - Red Button
13 - *Not used (bad pin)
A0 - Axis X
A1 - Axis Y
A2 - Axis X (2nd)
A3 - Axis Y (2nd)
A4 - Turbo mode (potentiometer)
A5 - Free

# Sites

PluggableUSB and PluggableHID howto - arduino/Arduino Wiki
https://github-wiki-see.page/m/arduino/Arduino/wiki/PluggableUSB-and-PluggableHID-howto
