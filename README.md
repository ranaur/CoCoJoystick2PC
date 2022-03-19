# CoCoJoystick2USB

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

The project will use HID-Project (2.8.2). To add it go in the Arduino IDE menu: Tools/Manage Libraries ... Search for HID-Project and install it.

## Files on Main project

Since the second prototype, things got a little bit more complex and organized. Lets describe the main classes/modules:

CoCoJoystick2USB.ino => main project file. Quite simple, because it instantiates all the classes that handles everything that is needed. The real job is done by CoCoJoystick class.

CoCoJoystick.cpp/.h => handles one Joystick. The setup() method defines which pins will be used for reading the joystick info (X Axis, Y Axis, Button 1 and Button 2) and the offset of the calibration data. The loop() method handle the reading of the pins and the delegation to a handler function. It also handles the calibration routines (startCalibration, centerCalibration and endCalibration) and loading/saving it to EEPROM. The calibration is done in two phases. First phase it read the maximum/minimum of the analog joystick (the user should move the stick as far as he can in all directions). The second phase tests for the center. The user cam move it slightly to handle any vibration of the stick. Or leave it in the center. If the stick goes back to any place of this region it will be considered as "center". The calibration data is saved on the EEPROM in a offset address.
	The reading of values are done by two classes. The stick (analog) is read by two AxisEvent instances (one for X and one for Y). The buttons are handled by two instances of ButtonEvent class.

\*Event Classes => While they are different classes, both share the same paradigm. Run setup() to configure parameters (i.e. pin number, tolecance, EEPROM Offset of calibration data ...) on the loop() routine of the microprocessor call the on\*() classes, that will handle reading, and, if necessary, call the first parameter, that is a callback function. Also, the on\*() methods can receive one pointer to pass to the callback function (Normally the event object itself, but it ca be anything), and the timer (optional). The timer parameter exist to call mills() just once and reuse the value through many calls in the loop() routine. Best use example is CoCoJoystick::loop().

AxisEvent.cpp/.h => Handles on axis. Reads the pin (0 to 1023) and normalize taking account of the calibration values. Handles the calibration state, aswell. The method onChanged() fires the callback if the value read from the sensor differs for more than the "tolerance" parameter. It handles the calibration routine aswell.

ButtonEvent.cpp/.h => Handles one button, with debounce. Has methods onPressed, onReleased, onPressedFor, onReleasedFor that calls the callback function on a button press (KEYDOWN), on a button release (KEYUP), after forMs miliseconds of the button press, and the same for the releade

Calibration.cpp/.h => the Calibration Objects encapsulates the real access to EEPROM, and the mapping from real values to adjusted (calibrated) values.

CoCoGamepadConfig.h => Abstract class that represents the interface of the events that can happen with a CoCo Gamepad could do. (press and release of the buttons, axis change). Implementing different 
MainAxisUSBGamepadConfig.h
SerialPrintGamepadConfig.h



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
	Less unuseful debug info.
	Detection Routines added. (needs extre wire)

# TODO

* Allow more than one joystick (learn about Multiple Reports or use Gamepad 1 to 4 to handle it)
	* with two joysticks make a switch/button to invert them
	* if you press the button for a long time (3 sec) and have only one joystick, make if double (act like to USB joysticks at the same time)
	* Save the setting on EEPROM?

* Allow changing the USB Behavior to allow different USB outputs: (make it a #define, switches on board, or a button to cycle between the options)
	* Main Axis and Buttons 0 and 1 (OK)
	* D-Pad and Buttons 0 and 1
	* P-Hat and Buttons 0 and 1
	* with 2 joysticks, left Joy to the left axis control, right joy to right axis control.
	* Criar USBGamepadConfigs:
	*    Red => 1 Black => 2 X => AxisX Y => AxisY
	*    Red => 1 Black => 1 X => AxisX Y => AxisY
	*    Red => 12346 Black => 12346 X => AxisX Y => AxisY
	*    Red => 1 Black => 2 X => RightAxisX => RightAxisY
	*    Red => 1 Black => 2 X => LeftAxisX => LeftAxisY
	*        (idem para o 11)

* Allow a complete calibration reset if press the calibration button for more than 10 sec

