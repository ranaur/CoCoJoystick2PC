#ifndef CoCoGamepadConfig_h
#define CoCoGamepadConfig_h

#include "Arduino.h"

class CoCoGamepadConfig {
  public:
  CoCoGamepadConfig() {};
  
  virtual void joystickConnected();
  virtual void joystickDisonnected();
  virtual void btnRedPress();
  virtual void btnBlackPress();
  virtual void btnRedRelease();
  virtual void btnBlackRelease();
  virtual void axisXchange(int value);
  virtual void axisYchange(int value);

  virtual void commit();
};

#endif