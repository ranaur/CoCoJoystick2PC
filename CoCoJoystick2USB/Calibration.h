/*
  Calibration.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef Calibration_h
#define Calibration_h

#include "Arduino.h"

struct maxmin {
	int maximum;
	int minimum; 
};

class Calibration
{
	static const int DEFAULT_MINIMUM = -32767;
	static const int DEFAULT_CENTER = 0;
	static const int DEFAULT_MAXIMUM = 32767;
	
  public:
	static const int EEPROMfootprint = sizeof(struct maxmin) * 2;

	Calibration();
	void setOutput(int minimum, int maximum, int center);
	void setEdge(int minimum, int maximum);
	void setCenter(int minimum, int maximum);
	void setEEPROMOffset(int EEPROMoffset);
	void saveEEPROM();
	void loadEEPROM();
  void printOutput();
  void printCalibration();
	int map(int real);
  
  private:
	int _EEPROMOffset;
	struct maxmin _output;
	int _outputCenter;
	struct maxmin _edge;
	struct maxmin _center;
};

#endif
