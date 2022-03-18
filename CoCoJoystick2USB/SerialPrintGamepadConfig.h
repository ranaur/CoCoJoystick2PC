#ifndef SerialPrintGamepadConfig_h
#define SerialPrintGamepadConfig_h

#include "CoCoGamepadConfig.h"

class SerialPrintGamepadConfig : public CoCoGamepadConfig {
  public:
  
  SerialPrintGamepadConfig() {
    Serial.println("SerialPrintGamepadConfig: constructor()");
    _changed = false;
  }
  
  void joystickConnected() { _changed = true; Serial.println("SerialPrintGamepadConfig: joystickConnected()"); };
  void joystickDisonnected() { _changed = true; Serial.println("SerialPrintGamepadConfig: joystickDisonnected()"); };
  void btnRedPress() { _changed = true; Serial.println("SerialPrintGamepadConfig: btnRedPress()"); }
  void btnRedRelease() { _changed = true; Serial.println("SerialPrintGamepadConfig: btnRedRelease()"); }
  void btnBlackPress()  { _changed = true; Serial.println("SerialPrintGamepadConfig: btnBlackPress()"); }
  void btnBlackRelease() { _changed = true; Serial.println("SerialPrintGamepadConfig: btnBlackRelease()"); }
  void axisXchange(int value) { _changed = true; Serial.print("SerialPrintGamepadConfig: axisXchange("); Serial.print(value); Serial.println(")"); };
  void axisYchange(int value) { _changed = true; Serial.print("SerialPrintGamepadConfig: axisYchange("); Serial.print(value); Serial.println(")"); };

  void commit() { if(_changed) { Serial.println("SerialPrintGamepadConfig: commit()"); _changed = false; } }

  private:
  bool _changed;
  
};
#endif
