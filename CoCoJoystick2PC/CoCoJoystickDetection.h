/*
  CocoJoystickDetection.h - Library for reading joystick.
  Created by Gustavo Schoenaker

  The detection if made testing the response of the outer shell (mesh) connected to a pin. If the shell is grounded, there is a joystick cconnected.
*/
#ifndef CoCoJoystickDetection_h
#define CoCoJoystickDetection_h

#include "Arduino.h"

#define null (void *)0

class CoCoJoystickDetection
{
  private:
  int _pin;
  int _status; // Joystick is inserted?
  bool detectJoystick();
  
  public:
  CoCoJoystickDetection();

  void setup(int pin);
  bool loop(void(*onConnect)(void *), void(*onDisconnect)(void *), void *obj = null);
};

#endif
