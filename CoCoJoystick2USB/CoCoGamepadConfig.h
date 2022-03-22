#ifndef CoCoGamepadConfig_h
#define CoCoGamepadConfig_h

#include "config.h"
#include "Arduino.h"

class CoCoGamepadConfig {
  public:
  CoCoGamepadConfig() {};
  
  virtual void joystickConnected();
  virtual void joystickDisconnected();
  virtual void btnRedPress();
  virtual void btnBlackPress();
  virtual void btnRedRelease();
  virtual void btnBlackRelease();
  virtual void axisXchange(int value);
  virtual void axisYchange(int value);

  virtual void commit();
};

#endif
