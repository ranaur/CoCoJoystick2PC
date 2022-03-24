/*
  CocoJoystick.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef CoCoJoystick_h
#define CoCoJoystick_h

#include "config.h"
#include "Arduino.h"

#ifdef CALIBRATION
#define COCOJOYSTICK_PERSISTENCE
#endif

#include "AxisEvent.h"
#include "ButtonEvent.h"
#include "TimerEvent.h"
#include "CoCoJoystickEvent.h"
#include "CoCoJoystickDetection.h"

class CoCoJoystick
{
  private:
      // Sensors
    CoCoJoystickDetection *detector;
    AxisEvent _axisX;
    AxisEvent _axisY;
    ButtonEvent _buttonRed;
    ButtonEvent _buttonBlack;
    
#ifdef CALIBRATION
    TimerEvent _calibrationTimeout;
    
    static const int EEPROM_DataFootprint = sizeof(mappingData_t) * 2; // XXX
    static const int EEPROM_CRCFootprint = sizeof(unsigned long);
    
    int _EEPROMOffset;

    enum STATE {
      OPERATING = 0,
      CALIBRATING_EDGES = 1,
      CALIBRATING_CENTERS = 2,
    };
    enum STATE _state;
#endif

#ifdef COCOJOYSTICK_PERSISTENCE
    //static const int EEPROMfootprint = sizeof(int) * 4 * 2 + sizeof(unsigned long);
  
      // save and loading from EEPROM
    unsigned long calculateEEPROMCRC();
    void saveCRC();
#endif
      // Events
    static void pressRed(uint32_t forMs, void *obj);
    static void pressBlack(uint32_t forMs, void *obj);
    static void releaseRed(uint32_t forMs, void *obj);
    static void releaseBlack(uint32_t forMs, void *obj);
    static void changeX(int value, void *obj);
    static void changeY(int value, void *obj);
  public:
      // Output
    CoCoJoystickEvent *_config;

  	CoCoJoystick();
  
  	void setup(int pinAxisX, int pinAxisY, int pinButtonRed, int pinButtonBlack, int pinShell, int EEPROMOffset = 0);
  	void loop(uint32_t now = millis());
  	void setConfig(CoCoJoystickEvent *config) { _config = config; };

#ifdef CALIBRATION
      // Calibration process
    static void calibrationTimeout(void *obj);
    enum STATE state() { return _state; };
    void startCalibration();
    void centerCalibration();
    void endCalibration();
    void resetCalibration();
    void cancelCalibration();
#endif

#ifdef COCOJOYSTICK_PERSISTENCE
      // save and loading from EEPROM
    void setEEPROMOffset(int EEPROMOffset) { _EEPROMOffset = EEPROMOffset; };
    bool checkCRC();
    void saveCalibration();
    void loadCalibration();
#endif
};

#endif