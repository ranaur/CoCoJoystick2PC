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

