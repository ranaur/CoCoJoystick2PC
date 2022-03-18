/*
  CocoJoystick.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef CoCoJoystick_h
#define CoCoJoystick_h

#include "Arduino.h"

#include <EEPROM.h>
#include "AxisEvent.h"
#include "ButtonEvent.h"
#include "CoCoGamepadConfig.h"

class CoCoJoystick
{
  public:
	static const int EEPROMfootprint = sizeof(Calibration) + sizeof(Calibration) + sizeof(unsigned long);

	CoCoJoystick();

	void setup(int pinAxisX, int pinAxisY, int pinButtonRed, int pinButtonBlack, int EEPROMOffset);
	void loop(uint32_t now = millis());
	void setConfig(CoCoGamepadConfig *config) { _config = config; };

  void loadCalibration();
  void saveCalibration();
  
  void startCalibration();
  void centerCalibration();
  void endCalibration();

  enum STATE {
    OPERATING = 0,
    CALIBRATING_EDGES = 1,
    CALIBRATING_CENTERS = 2,
  };
  enum STATE state() { return _state; };
  CoCoGamepadConfig *_config;
  
  private:
  enum STATE _state;
	int _pinAxisX;
	int _pinAxisY;
	int _pinButtonRed;
	int _pinButtonBlack;
	int _EEPROMOffset;

	AxisEvent _axisX;
	AxisEvent _axisY;
	ButtonEvent _buttonRed;
	ButtonEvent _buttonBlack;
  
  unsigned long calculateEEPROMCRC();
  int EEPROMCRCOffset();
	bool checkEEPROM();
  static void press(void *obj);
  static void release(void *obj);
};

#endif
