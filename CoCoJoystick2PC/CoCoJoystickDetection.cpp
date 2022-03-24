#include "CoCoJoystickDetection.h"

CoCoJoystickDetection::CoCoJoystickDetection() {
  _pin = -1;
}

bool CoCoJoystickDetection::detectJoystick() {
  if(_pin == -1) return true; // no pin, always return as detected
  return !digitalRead(_pin);
}

void CoCoJoystickDetection::setup(int pin) {
  _pin = pin;

  pinMode(_pin, INPUT_PULLUP);

  _status = detectJoystick();
}

bool CoCoJoystickDetection::loop(void(*onConnect)(void *), void(*onDisconnect)(void *), void *obj = null) {

  bool hasJoystick = detectJoystick();

  if(hasJoystick != _status) {
    _status = hasJoystick;
    if(_status) {
      onConnect(obj);
    } else {
      onDisconnect(obj);
    }
  }

  return _status;
}
  
