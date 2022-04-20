#include "CoCoJoystick.h"

#include <EEPROM.h>

#define COCOJOYSTICK_AXIS_TOLERANCE 16

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

void printEEPROM(int start, int count) {
 Serial.print("EEPROM[");
 Serial.print(start);
 Serial.print("] = ");
 for (int n = start; n <= start + count; n++)
  {
    Serial.print(EEPROM[n] < 16 ? "0" : "");
    Serial.print(EEPROM[n], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

static void CoCoJoystick::joystickConnected(void *obj) {
  ((CoCoJoystick *)obj)->_config->joystickConnected();
}

static void CoCoJoystick::joystickDisconnected(void *obj) {
  ((CoCoJoystick *)obj)->_config->joystickDisconnected();
}

CoCoJoystick::CoCoJoystick() {
  setEEPROMOffset(-1);
}

void CoCoJoystick::setup(int pinAxisX, int pinAxisY, int pinButtonRed, int pinButtonBlack, int pinShell, int EEPROMOffset = -1)
{
  debugfunction("CoCoJoystick::setup(...)");

#ifdef DETECT_JOYSTICK
  //_config->joystickConnected(); // XXX use detection routines
  detector = new CoCoJoystickDetection();
  detector->setup(pinShell);
#endif
  
	_EEPROMOffset = EEPROMOffset;

  _axisX.setup(pinAxisX, COCOJOYSTICK_AXIS_TOLERANCE);
  _axisX.setOutput(_config->getMinAxisX(), _config->getMaxAxisX(), _config->getCenterX());
  _axisY.setup(pinAxisY, COCOJOYSTICK_AXIS_TOLERANCE);
  _axisY.setOutput(_config->getMinAxisY(), _config->getMaxAxisY(), _config->getCenterY());

  setEEPROMOffset(EEPROMOffset);
  debugvar("EEPROMOffset = ", EEPROMOffset); debugln("");
  
  if(EEPROMOffset != -1) {
    _calibrationTimeout.setup();
    loadCalibration();
  } else {
    _axisX.setDefault();
    _axisY.setDefault();
  }

	_buttonRed.setup(pinButtonRed);
	_buttonBlack.setup(pinButtonBlack);
}

static void CoCoJoystick::pressRed(uint32_t forMs, void *obj) {
  debugfunction("CoCoJoystick::pressRed(...)");

  if(((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_CENTERS && forMs > 500) {
    ((CoCoJoystick *)obj)->endCalibration();
  }
  if(((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_EDGES && forMs > 500) {
    ((CoCoJoystick *)obj)->centerCalibration();
  }

  ((CoCoJoystick *)obj)->_config->btnRedPress();
}

void CoCoJoystick::pressBlack(uint32_t forMs, void *obj) {
  debugfunction("CoCoJoystick::pressBlack(...)");

  if((((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_CENTERS || ((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_EDGES) && forMs > 500) {
    ((CoCoJoystick *)obj)->cancelCalibration();
  }
  
  ((CoCoJoystick *)obj)->_config->btnBlackPress();
}

void CoCoJoystick::releaseRed(uint32_t forMs, void *obj) {
  debugfunction("CoCoJoystick::releaseRed(...)");
  ((CoCoJoystick *)obj)->_config->btnRedRelease();
}

void CoCoJoystick::releaseBlack(uint32_t forMs, void *obj) {
  debugfunction("CoCoJoystick::releaseBlack(...)");
  ((CoCoJoystick *)obj)->_config->btnBlackRelease();
}

void CoCoJoystick::changeX(int value, void *obj) {
  debugfunction("CoCoJoystick::changeX(...)");
  ((CoCoJoystick *)obj)->_config->axisXchange(value);
}

void CoCoJoystick::changeY(int value, void *obj) {
  debugfunction("CoCoJoystick::changeY(...)");
  ((CoCoJoystick *)obj)->_config->axisYchange(value);
}

void CoCoJoystick::loop(uint32_t now = millis()) {
  _calibrationTimeout.loop(now);
  _calibrationTimeout.onTimer(60000, calibrationTimeout, this);

#ifdef DETECT_JOYSTICK
  if(! detector->loop(joystickConnected, joystickDisconnected, this) ) return;
#endif
  
	_buttonRed.loop(now);
	_buttonRed.onPressed(pressRed, this);
	_buttonRed.onReleased(releaseRed, this);

	_buttonBlack.loop(now);
  _buttonBlack.onPressed(pressBlack, this);
	_buttonBlack.onReleased(releaseBlack, this);

  _axisX.onChanged(changeX, this, now);
  _axisY.onChanged(changeY, this, now);

  _config->commit();
  // implementar inversão de joysticks/modo _calibrateButton.onRelease(..., 0);
  // implementar calibragem _calibrateButton.onRelease(..., 3000);
}

void CoCoJoystick::calibrationTimeout(void *obj) {
  ((CoCoJoystick *)obj)->cancelCalibration();

  debugln("CALIBRATION TIMEOUT");
}

void CoCoJoystick::startCalibration(void(*centerCallback)(CoCoJoystick *) = null, void(*finishCallback)(CoCoJoystick *) = null) {
  _centerCallback = centerCallback;
  _finishCallback = finishCallback;
  _state = STATE::CALIBRATING_EDGES;
  _axisX.startCalibration();
  _axisY.startCalibration();
  _calibrationTimeout.restart();

  delay(100);
  debugln("BEGIN CALIBRATION ROUTINE.");
  debugln("Move joystick to maximum points and press the Red Button.");
}

void CoCoJoystick::centerCalibration() {
  _state = STATE::CALIBRATING_CENTERS;
  _axisX.centerCalibration();
  _axisY.centerCalibration();
  _calibrationTimeout.restart();

  delay(100);
  if(_centerCallback) _centerCallback(this); else Serial.println("NO CALLBACK for CANCEL");
  debugln("CALIBRATE CENTER ROUTINE.");
  debugln("Set joystick to center point and press the Red Button.");
}

void CoCoJoystick::endCalibration() {
  _state = STATE::OPERATING;
  _axisX.endCalibration();
  _axisY.endCalibration();
  _calibrationTimeout.disable();
  
  saveCalibration();

    // put the last read with the new calibration
  changeX(_axisX.value(), this);
  changeY(_axisY.value(), this);
  
  delay(100);
  if(_finishCallback) _finishCallback(this);
  debugln("CALIBRATION ROUTINE ENDED.");
}

void CoCoJoystick::resetCalibration() {
  _state = STATE::OPERATING;
  _axisX.resetCalibration();
  _axisY.resetCalibration();
  _calibrationTimeout.disable();

  saveCalibration();

    // put the last read with the new calibration
  changeX(_axisX.value(), this);
  changeY(_axisY.value(), this);

  debugln("CALIBRATION RESET.");
}

void CoCoJoystick::cancelCalibration() {
  _state = STATE::OPERATING;
  _calibrationTimeout.disable();

  _axisX.cancelCalibration();
  _axisY.cancelCalibration();
  
  changeX(_axisX.value(), this);
  changeY(_axisY.value(), this);

  if(_finishCallback) _finishCallback(this);
  debugln("CALIBRATION CANCELED.");
}


unsigned long CoCoJoystick::calculateEEPROMCRC() {
  if(_EEPROMOffset == -1) { return false; };
  return eeprom_crc(_EEPROMOffset, EEPROM_DataFootprint);
}

void CoCoJoystick::saveCRC() {
  if(_EEPROMOffset == -1) { return; };

  unsigned long crc = calculateEEPROMCRC();
  EEPROM.put(_EEPROMOffset + EEPROM_DataFootprint, crc);
}

bool CoCoJoystick::checkCRC() {
  if(_EEPROMOffset == -1) { return false; };

  unsigned long calculatedCRC = calculateEEPROMCRC();

  unsigned long savedCRC;
  EEPROM.get(_EEPROMOffset + EEPROM_DataFootprint, savedCRC);

  return savedCRC == calculatedCRC;
}

void CoCoJoystick::saveCalibration() {
  debugfunction("CoCoJoystick::saveCalibration()");
  if(_EEPROMOffset == -1) { return; };

  EEPROM.put(_EEPROMOffset, *_axisX.getInput());
  EEPROM.put(_EEPROMOffset + sizeof(mappingData_t), *_axisY.getInput());

  saveCRC();
  
  loadCalibration(); // just to be sure
}

void CoCoJoystick::loadCalibration() {
  debugfunction("CoCoJoystick::loadCalibration()");
  if(_EEPROMOffset == -1) { 
    debugln("EEPROM Offset not set");
    _axisX.setDefault();
    _axisY.setDefault();
    return;
  };

  if(checkCRC()) {
    debugln("Loading calibration from EEPROM");
    mappingData_t dataX;
    EEPROM.get(_EEPROMOffset, dataX);
    _axisY.setInput(dataX);
    
    mappingData_t dataY;
    EEPROM.get(_EEPROMOffset + sizeof(mappingData_t), dataY);
    _axisY.setInput(dataY);
  } else {
     debugln("Loading default calibration");
    _axisX.setDefault();
    _axisY.setDefault();
  }
}
