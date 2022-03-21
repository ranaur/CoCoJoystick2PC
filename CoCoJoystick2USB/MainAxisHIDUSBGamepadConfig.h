#ifndef MainAxisHIDUSBGamepadConfig_h
#define MainAxisHIDUSBGamepadConfig_h

#include "CoCoGamepadConfig.h"

#include "HIDGamepad.h"

class MainAxisHIDUSBGamepadConfig : public CoCoGamepadConfig {
  private:
  HIDGamepad_ *_gamepad;
  int _buttonRed;
  int _buttonBlack;
  
  public:

  MainAxisHIDUSBGamepadConfig() {
    setGamepad(&HIDGamepad); // XXX allow multiple gamepads!
    setButtonNumbers(1, 2); // make if configurable?
  };
  
  void setButtonNumbers(int buttonRed, int buttonBlack ) {
    _buttonRed = buttonRed;
    _buttonBlack = buttonBlack;
  }
  
  void joystickConnected() { _gamepad->begin(); };
  void joystickDisconnected() { _gamepad->end(); };

  void btnRedPress() { _gamepad->pressRed(); }
  void btnRedRelease() { _gamepad->releaseRed(); }
  void btnBlackPress()  { _gamepad->pressBlack(); }
  void btnBlackRelease() { _gamepad->releaseBlack(); }
  void axisXchange(int value) { _gamepad->xAxis(value); };
  void axisYchange(int value) { _gamepad->yAxis(value); };

  void setGamepad(HIDGamepad_ *gamepad) { _gamepad = gamepad; }

  void commit() { _gamepad->write(); }
};

#endif
