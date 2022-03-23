/**
 * AxisEvent.h - Read an analog pin and propagate an event on any change
 * Created by Gustavo Schoenaker on 2022
 * 
 * #DFFINES:
 *  CALIBRATION: it will allow calibration routines
 *  DEBUG_AXISEVENT: show verbose results on serial port
 * 
 * FUNCTIONS:
 *  setup() => must be called on setup() function of the main program to configure the axis pin
 *  onChanged() => must be called on loop() function of the main program
 *  loop() => does nothing
 *  
 */
#ifndef AxisEvent_h
#define AxisEvent_h

#include "config.h"
#include "Arduino.h"

#include "Mapping.h"

class AxisEvent {
  private:
      // defaults if EEPROM does not have consistent data
    static const int defaultInputMaximum = 1023; // 10 bit DAC (From Leonardo)
    static const int defaultInputMinimum = 0;
    static const int defaultInputCenterDelta = 16;
    static const int defaultInputCenterMinium = (defaultInputMaximum + defaultInputMinimum) / 2 - defaultInputCenterDelta;
    static const int defaultInputCenterMaximum = (defaultInputMaximum + defaultInputMinimum) / 2 + defaultInputCenterDelta;
    static const int defaultOutputMinimum = -32767; // USB Reference
    static const int defaultOutputCenter = 0;
    static const int defaultOutputMaximum = 32767;
  
      // Config vars
    int _pin;
    int _tolerance;

    int _lastValue;
    Mapping mapping;

  public:
    AxisEvent();

    void setup(int pin, int tolerance);
    void setOutput(int minimum, int maximum, int center);
    void setDefault();
    void loop() {};
    
    int rawValue() { return _lastValue; }
    int value() { return mapping.map(rawValue()); }
    
    void onChanged(void(*callback)(int, void *), void *obj = (void *)0, uint32_t now = millis());

    void setInput(mappingData_t newValue) { mapping.setInput(newValue); };
    mappingData_t *getInput() { return mapping.getInput(); };

  protected:

  public:
#ifdef CALIBRATION
    static const int EEPROMSize = sizeof(int) * 4;
    void setEEPROMOffset(int EEPROMOffset);
    void startCalibration();
    void centerCalibration();
    void endCalibration();
    void printCalibration();
    void resetCalibration();
    void cancelCalibration();
    int centerValue() { return mapping.centerValue(); };
#endif

#ifdef CALIBRATION
    int _EEPROMOffset;
    mappingData_t tempCalibration;

    enum STATE {
      OPERATING = 0,
      CALIBRATING_EDGES = 1,
      CALIBRATING_CENTERS = 2,
    } _state;
#endif
};

#endif
