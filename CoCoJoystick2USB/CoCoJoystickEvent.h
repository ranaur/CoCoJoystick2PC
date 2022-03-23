#ifndef CoCoJoystickEvent_h
#define CoCoJoystickEvent_h

#include "config.h"
#include "Arduino.h"

class CoCoJoystickEvent {
  public:
  CoCoJoystickEvent() {};

  int getMinAxisX() { return -32768; };
  int getCenterX() { return 0; };
  int getMaxAxisX() { return 32767; };
  int getMinAxisY() { return -32768; };
  int getCenterY() { return 0; };
  int getMaxAxisY() { return 32767; };
  
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
