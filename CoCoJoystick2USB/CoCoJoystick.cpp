#include "Arduino.h"
#include "CoCoJoystick.h"

//#include "SerialPrintGamepadConfig.h"
//#include "MainAxisUSBGamepadConfig.h"
#include "MainAxisHIDUSBGamepadConfig.h"

#include <EEPROM.h>

unsigned long eeprom_crc(int start, int length) {
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;
  for (int index = start ; index < start + length; ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

void joystickConnected(void *obj) {
  ((CoCoJoystick *)obj)->_config->joystickConnected();
}

void joystickDisconnected(void *obj) {
  ((CoCoJoystick *)obj)->_config->joystickDisconnected();
}

CoCoJoystick::CoCoJoystick() {
	_pinAxisX = -1;
	_pinAxisY = -1;
	_pinButtonRed = -1;
	_pinButtonBlack = -1;
	_EEPROMOffset = -1;
  //setConfig(new SerialPrintGamepadConfig());
  //setConfig(new MainAxisUSBGamepadConfig()); // Allow change the behaviour of the GamePad (try #defines, then go for a switch)
  setConfig(new MainAxisHIDUSBGamepadConfig()); // Allow change the behaviour of the GamePad (try #defines, then go for a switch)
}

bool CoCoJoystick::checkEEPROM() {
  if(_EEPROMOffset == -1) { return false; };

  unsigned long calculatedCRC = calculateEEPROMCRC();

  unsigned long savedCRC;
  EEPROM.get(EEPROMCRCOffset(), savedCRC);

  return savedCRC == calculatedCRC;
}

int CoCoJoystick::EEPROMCRCOffset() {
  return _EEPROMOffset + EEPROMfootprint - sizeof(unsigned long);
}

unsigned long CoCoJoystick::calculateEEPROMCRC() {
  if(_EEPROMOffset == -1) { return false; };
  return eeprom_crc(_EEPROMOffset, EEPROMfootprint - sizeof(unsigned long));
}

void CoCoJoystick::loadCalibration() {
	if(_EEPROMOffset == -1) { return; };

	if(checkEEPROM()) {
#ifdef DEBUG
    Serial.println("Loading calibration from EEPROM");
#endif
    _axisX.loadCalibration();
    _axisY.loadCalibration();
	} else {
#ifdef DEBUG
    Serial.println("Loading default calibration");
#endif
    _axisX.setDefaultCalibration();
    _axisY.setDefaultCalibration();
    //saveCalibration();
	}

#ifdef DEBUG
  Serial.println("Axis X Calibration:");
  _axisX.printCalibration();
  Serial.println("Axis Y Calibration:");
  _axisY.printCalibration();
#endif
}

void CoCoJoystick::saveCalibration() {
	if(_EEPROMOffset == -1) { return; };

	_axisX.saveCalibration();
	_axisY.saveCalibration();
  
  unsigned long crc = calculateEEPROMCRC();
  EEPROM.put(EEPROMCRCOffset(), crc);
}

void CoCoJoystick::setup(int pinAxisX, int pinAxisY, int pinButtonRed, int pinButtonBlack, int pinShell, int EEPROMOffset)
{
  //_config->joystickConnected(); // XXX use detection routines
  detector = new CoCoJoystickDetection();
  detector->setup(pinShell);
  
	_pinAxisX = pinAxisX;
	_pinAxisY = pinAxisY;
	_pinButtonRed = pinButtonRed;
	_pinButtonBlack = pinButtonBlack;

	_EEPROMOffset = EEPROMOffset;

  _axisX.setup(pinAxisX, EEPROMOffset, 16);
  _axisY.setup(pinAxisY, EEPROMOffset + Calibration::EEPROMfootprint, 16);
  
  loadCalibration();
  
	_buttonRed.setup(pinButtonRed);
	_buttonBlack.setup(pinButtonBlack);
}

void pressRed(uint32_t forMs, void *obj) {
  if(((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_CENTERS && forMs > 500) {
    ((CoCoJoystick *)obj)->endCalibration();
  }
  if(((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_EDGES && forMs > 500) {
    ((CoCoJoystick *)obj)->centerCalibration();
  }
  ((CoCoJoystick *)obj)->_config->btnRedPress();
}

void pressBlack(uint32_t forMs, void *obj) {
  ((CoCoJoystick *)obj)->_config->btnBlackPress();
}

void releaseRed(uint32_t forMs, void *obj) {
  ((CoCoJoystick *)obj)->_config->btnRedRelease();
}

void releaseBlack(uint32_t forMs, void *obj) {
  ((CoCoJoystick *)obj)->_config->btnBlackRelease();
}

void changeX(int value, void *obj) {
  ((CoCoJoystick *)obj)->_config->axisXchange(value);
}

void changeY(int value, void *obj) {
  ((CoCoJoystick *)obj)->_config->axisYchange(value);
}

void CoCoJoystick::loop(uint32_t now = millis()) {
  if(! detector->loop(joystickConnected, joystickDisconnected, this) ) return;
  
	_buttonRed.loop(now);
	_buttonRed.onPressed(::pressRed, this);
	_buttonRed.onReleased(::releaseRed, this);

	_buttonBlack.loop(now);
  _buttonBlack.onPressed(::pressBlack, this);
	_buttonBlack.onReleased(::releaseBlack, this);

  _axisX.onChanged(::changeX, this, now);
  _axisY.onChanged(::changeY, this, now);

  _config->commit();
  // implementar inversão de joysticks/modo _calibrateButton.onRelease(..., 0);
  // implementar calibragem _calibrateButton.onRelease(..., 3000);
}

void CoCoJoystick::startCalibration() {
  _state = STATE::CALIBRATING_EDGES;
  _axisX.startCalibration();
  _axisY.startCalibration();

#ifdef DEBUG
  Serial.println("BEGIN CALIBRATION ROUTINE.");
  Serial.println("Move joystick to maximum points.");
#endif
}

void CoCoJoystick::centerCalibration() {
  _state = STATE::CALIBRATING_CENTERS;
  _axisX.centerCalibration();
  _axisY.centerCalibration();
#ifdef DEBUG
  Serial.println("CALIBRATE CENTER ROUTINE.");
  Serial.println("Set joystick to center point.");
#endif
}

void CoCoJoystick::endCalibration() {
  _state = STATE::OPERATING;
#ifdef DEBUG
  Serial.println("Axis X:");
#endif
  _axisX.endCalibration();
#ifdef DEBUG
  Serial.println("Axis Y:");
#endif
  _axisY.endCalibration();
  saveCalibration();
#ifdef DEBUG
  Serial.println("CALIBRATION ROUTINE ENDED.");
#endif
}
