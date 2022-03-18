/*
  AxisEvent.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef AxisEvent_h
#define AxisEvent_h

#include "Arduino.h"
#include "Calibration.h"

class AxisEvent {
  public:
    AxisEvent();
    void setup(int pin, int EEPROMOffset, int tolerance);
    
    void onChanged(void(*callback)(int, void *), void *obj = (void *)0, uint32_t now = millis());
    
    void loadCalibration();
    void saveCalibration();
    void setDefaultCalibration();
    void startCalibration();
    void centerCalibration();
    void endCalibration();
    void printCalibration();
  private:
      // defaults if EEPROM does not have consistent data
    static const int defaultMaximum = 1023;
    static const int defaultMinimum = 0;
    static const int defaultCenterDelta = 16;
    static const int defaultCenterMinium = (defaultMaximum + defaultMinimum) / 2 - defaultCenterDelta;
    static const int defaultCenterMaximum = (defaultMaximum + defaultMinimum) / 2 + defaultCenterDelta;

      // Config vars
    int _pin;
    int _tolerance;

      // Operational Vars
    int _lastValue;
    Calibration calibration;
    struct {
      struct {
        int minimum;
        int maximum;
      } _edge;
      struct {
        int minimum;
        int maximum;
      } _center;
    } _temporaryCalibration;
    enum STATE {
      OPERATING = 0,
      CALIBRATING_EDGES = 1,
      CALIBRATING_CENTERS = 2,
    } _state;
};

#endif
