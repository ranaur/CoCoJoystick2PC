#ifndef MainAxisUSBGamepadConfig_h
#define MainAxisUSBGamepadConfig_h
// Track if any value changed on write?
#include "CoCoGamepadConfig.h"

#include <HID-Project.h>
#include <HID-Settings.h>

class MainAxisUSBGamepadConfig : public CoCoGamepadConfig {
  private:
  SingleGamepad_ *_gamepad;
  int _buttonRed;
  int _buttonBlack;
  
  public:

  MainAxisUSBGamepadConfig() {
    setGamepad(&Gamepad1); // XXX allow multiple gamepads!
    setButtonNumbers(1, 2); // make if configurable?
  };
  
  void setButtonNumbers(int buttonRed, int buttonBlack ) {
    _buttonRed = buttonRed;
    _buttonBlack = buttonBlack;
  }
  
  void joystickConnected() { _gamepad->begin();  Serial.print("Gamepad BEGIN"); };
  void joystickDisonnected() { _gamepad->end(); };

  void btnRedPress() { _gamepad->press(_buttonRed);  Serial.print("Red Press"); Serial.println(_buttonRed); }
  void btnRedRelease() { _gamepad->release(_buttonRed);  Serial.print("Red Release"); Serial.println(_buttonRed);  }
  void btnBlackPress()  { _gamepad->press(_buttonBlack); Serial.print("Black Press"); Serial.println(_buttonBlack);  }
  void btnBlackRelease() { _gamepad->release(_buttonBlack); Serial.print("Black Release"); Serial.println(_buttonBlack); }
  void axisXchange(int value) { _gamepad->xAxis(value); };
  void axisYchange(int value) { _gamepad->yAxis(value); };

  void setGamepad(SingleGamepad_ *gamepad) { _gamepad = gamepad; }

  void commit() { _gamepad->write(); }
};

#endif
