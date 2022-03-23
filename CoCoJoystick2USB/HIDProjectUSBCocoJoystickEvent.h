#ifndef HIDProjectUSBCoCoJoystickEvent_h
#define HIDProjectUSBCoCoJoystickEvent_h

// TODO? Track if any value changed on write?

#include "CoCoGamepadConfig.h"

#include <HID-Project.h>
#include <HID-Settings.h>

class HIDProjectUSBCoCoJoystickEvent : public CoCoJoystickEvent {
  private:
  Gamepad_ *_gamepad;
  int _buttonRed;
  int _buttonBlack;
  
  public:

  HIDProjectUSBCoCoJoystickEvent() {
    setGamepad(&Gamepad); // XXX allow multiple gamepads!
    setButtonNumbers(1, 2); // make if configurable?
  };
  
  void setButtonNumbers(int buttonRed, int buttonBlack ) {
    _buttonRed = buttonRed;
    _buttonBlack = buttonBlack;
  }
  
  void joystickConnected() { _gamepad->begin(); };
  void joystickDisconnected() { _gamepad->end(); };

  void btnRedPress() { _gamepad->press(_buttonRed); }
  void btnRedRelease() { _gamepad->release(_buttonRed); }
  void btnBlackPress()  { _gamepad->press(_buttonBlack); }
  void btnBlackRelease() { _gamepad->release(_buttonBlack); }
  void axisXchange(int value) { _gamepad->xAxis(value); };
  void axisYchange(int value) { _gamepad->yAxis(value); };

  void setGamepad(Gamepad_ *gamepad) { _gamepad = gamepad; }

  void commit() { _gamepad->write(); }
};

#endif
