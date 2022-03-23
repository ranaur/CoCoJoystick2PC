#ifndef SerialPrintCoCoJoystickEvent_h
#define SerialPrintCoCoJoystickEvent_h

#include "CoCoJoystickEvent.h"

class SerialPrintCoCoJoystickEvent : public CoCoJoystickEvent {
  public:
  
  SerialPrintCoCoJoystickEvent() {
    Serial.println("SerialPrintCoCoJoystickEvent: constructor()");
    _changed = false;
  }
  
  void joystickConnected() { _changed = true; Serial.println("SerialPrintCoCoJoystickEvent: joystickConnected()"); };
  void joystickDisconnected() { _changed = true; Serial.println("SerialPrintCoCoJoystickEvent: joystickDisconnected()"); };
  void btnRedPress() { _changed = true; Serial.println("SerialPrintCoCoJoystickEvent: btnRedPress()"); }
  void btnRedRelease() { _changed = true; Serial.println("SerialPrintCoCoJoystickEvent: btnRedRelease()"); }
  void btnBlackPress()  { _changed = true; Serial.println("SerialPrintCoCoJoystickEvent: btnBlackPress()"); }
  void btnBlackRelease() { _changed = true; Serial.println("SerialPrintCoCoJoystickEvent: btnBlackRelease()"); }
  void axisXchange(int value) { _changed = true; Serial.print("SerialPrintCoCoJoystickEvent: axisXchange("); Serial.print(value); Serial.println(")"); };
  void axisYchange(int value) { _changed = true; Serial.print("SerialPrintCoCoJoystickEvent: axisYchange("); Serial.print(value); Serial.println(")"); };

  void commit() { if(_changed) { Serial.println("SerialPrintCoCoJoystickEvent: commit()"); _changed = false; } }

  private:
  bool _changed;
  
};
#endif
