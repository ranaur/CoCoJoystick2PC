#include "Calibration.h"

#include <EEPROM.h>

Calibration::Calibration() {
	setOutput(DEFAULT_MINIMUM, DEFAULT_CENTER, DEFAULT_MAXIMUM);
	setEEPROMOffset(-1);
}

void Calibration::setOutput(int minimum, int center, int maximum) {
	_output.minimum = minimum;
	_output.maximum = maximum;
	_outputCenter = center;
}

void Calibration::setEEPROMOffset(int EEPROMoffset) {
	_EEPROMOffset = EEPROMoffset;
}

void Calibration::loadEEPROM() {
	if(_EEPROMOffset == -1) { return; };

	int offset = _EEPROMOffset;

	EEPROM.get(offset, _edge.minimum); offset += sizeof(_edge.minimum);
	EEPROM.get(offset, _edge.maximum); offset += sizeof(_edge.maximum);
	EEPROM.get(offset, _center.minimum); offset += sizeof(_center.minimum);
	EEPROM.get(offset, _center.maximum); offset += sizeof(_center.maximum);
  setEdge(_edge.minimum, _edge.maximum);
  setCenter(_center.minimum, _center.maximum);
}

void Calibration::saveEEPROM() {
	if(_EEPROMOffset == -1) { return; };
	int offset = _EEPROMOffset;

	EEPROM.put(offset, _edge.minimum); offset += sizeof(_edge.minimum);
	EEPROM.put(offset, _edge.maximum); offset += sizeof(_edge.maximum);
	EEPROM.put(offset, _center.minimum); offset += sizeof(_center.minimum);
	EEPROM.put(offset, _center.maximum); offset += sizeof(_center.maximum);
}

void Calibration::setEdge(int minimum, int maximum) {
	_edge.minimum = minimum;
	_edge.maximum = maximum;
}

void Calibration::setCenter(int minimum, int maximum) {
	_center.minimum = minimum;
	_center.maximum = maximum;
}

void Calibration::printOutput() {
  Serial.print("output = [ ");
  Serial.print(_output.minimum);
  Serial.print(", ");
  Serial.print(_output.maximum);
  Serial.print(" ] center = ");
  Serial.print(_outputCenter);
  Serial.println("");
}

void Calibration::printCalibration() {
  Serial.print("edge = [ ");
  Serial.print(_edge.minimum);
  Serial.print(", ");
  Serial.print(_edge.maximum);
  Serial.print(" ] center = [ ");
  Serial.print(_center.minimum);
  Serial.print(", ");
  Serial.print(_center.maximum);
  Serial.println(" ]");
}

int Calibration::map(int real) {
  int mapped;
	// return map(rawValueX, 0, 1023, _output.minimum, _output.maximum);
	if(real >= _center.minimum && real <= _center.maximum) {
	  mapped = _outputCenter; 
	  //Serial.print("C");
	}
	if(real < _center.minimum) {
	  mapped = ::map(real, _edge.minimum, _center.minimum, _output.minimum, _outputCenter); 
    //Serial.print("L");
	}
	if(real > _center.maximum) {
	  mapped = ::map(real, _center.maximum, _edge.maximum, _outputCenter, _output.maximum); 
	  //Serial.print("U");
	}
  //Serial.print("real = ");
  //Serial.println(real);
  //Serial.print("mapped = ");
  //Serial.println(mapped);
  return mapped;
}
