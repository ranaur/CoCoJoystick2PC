#include "CoCoJoystick.h"

#ifdef COCOJOYSTICK_PERSISTENCE
#include <EEPROM.h>
#endif

#define COCOJOYSTICK_AXIS_TOLERANCE 16
//#include "SerialPrintGamepadConfig.h"
//#include "MainAxisUSBGamepadConfig.h"
#include "HIDCoCoJoystickUSBGamepadConfig.h"

#ifdef COCOJOYSTICK_PERSISTENCE
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
  Serial.println();
}
#endif

void joystickConnected(void *obj) {
  ((CoCoJoystick *)obj)->_config->joystickConnected();
}

void joystickDisconnected(void *obj) {
  ((CoCoJoystick *)obj)->_config->joystickDisconnected();
}

CoCoJoystick::CoCoJoystick() {
  setEEPROMOffset(-1);
  //setConfig(new SerialPrintGamepadConfig());
  setConfig(new HIDCoCoJoystickUSBGamepadConfig()); // Allow change the behaviour of the GamePad (try #defines, then go for a switch)
}

void CoCoJoystick::setup(int pinAxisX, int pinAxisY, int pinButtonRed, int pinButtonBlack, int pinShell, int EEPROMOffset)
{
#ifdef DEBUG
  Serial.println("CoCoJoystick::setup(...)");
#endif

  //_config->joystickConnected(); // XXX use detection routines
  detector = new CoCoJoystickDetection();
  detector->setup(pinShell);
  
	_EEPROMOffset = EEPROMOffset;

  _axisX.setup(pinAxisX, COCOJOYSTICK_AXIS_TOLERANCE);
  _axisY.setup(pinAxisY, COCOJOYSTICK_AXIS_TOLERANCE);
    
#ifdef COCOJOYSTICK_PERSISTENCE
  setEEPROMOffset(EEPROMOffset);
  loadCalibration();
#else
  _axisX.setDefault();
  _axisY.setDefault();
#endif  

	_buttonRed.setup(pinButtonRed);
	_buttonBlack.setup(pinButtonBlack);
}

static void CoCoJoystick::pressRed(uint32_t forMs, void *obj) {
#ifdef DEBUG_EVENT
  Serial.println("CoCoJoystick::pressRed(...)");
#endif

#ifdef CALIBRATION
  if(((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_CENTERS && forMs > 500) {
    ((CoCoJoystick *)obj)->endCalibration();
  }
  if(((CoCoJoystick *)obj)->state() == CoCoJoystick::STATE::CALIBRATING_EDGES && forMs > 500) {
    ((CoCoJoystick *)obj)->centerCalibration();
  }
#endif

  ((CoCoJoystick *)obj)->_config->btnRedPress();
}

void pressBlack(uint32_t forMs, void *obj) {
#ifdef DEBUG_EVENT
  Serial.println("CoCoJoystick::pressBlack(...)");
#endif
  ((CoCoJoystick *)obj)->_config->btnBlackPress();
}

void releaseRed(uint32_t forMs, void *obj) {
#ifdef DEBUG_EVENT
  Serial.println("CoCoJoystick::releaseRed(...)");
#endif
  ((CoCoJoystick *)obj)->_config->btnRedRelease();
}

void releaseBlack(uint32_t forMs, void *obj) {
#ifdef DEBUG_EVENT
  Serial.println("CoCoJoystick::releaseBlack(...)");
#endif
  ((CoCoJoystick *)obj)->_config->btnBlackRelease();
}

void changeX(int value, void *obj) {
#ifdef DEBUG_EVENT
  Serial.println("CoCoJoystick::changeX(...)");
#endif
  ((CoCoJoystick *)obj)->_config->axisXchange(value);
}

void changeY(int value, void *obj) {
#ifdef DEBUG_EVENT
  Serial.println("CoCoJoystick::changeY(...)");
#endif
  ((CoCoJoystick *)obj)->_config->axisYchange(value);
}

void CoCoJoystick::loop(uint32_t now = millis()) {
  if(! detector->loop(joystickConnected, joystickDisconnected, this) ) return;
  
	_buttonRed.loop(now);
	_buttonRed.onPressed(pressRed, this);
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

#ifdef CALIBRATION
void CoCoJoystick::startCalibration() {
  _state = STATE::CALIBRATING_EDGES;
  _axisX.startCalibration();
  _axisY.startCalibration();

#ifdef DEBUG
  Serial.println("BEGIN CALIBRATION ROUTINE.");
  Serial.println("Move joystick to maximum points and press the Red Button.");
#endif
}

void CoCoJoystick::centerCalibration() {
  _state = STATE::CALIBRATING_CENTERS;
  _axisX.centerCalibration();
  _axisY.centerCalibration();
#ifdef DEBUG
  Serial.println("CALIBRATE CENTER ROUTINE.");
  Serial.println("Set joystick to center point and press the Red Button.");
#endif
}

void CoCoJoystick::endCalibration() {
  _state = STATE::OPERATING;
  _axisX.endCalibration();
  _axisY.endCalibration();
  saveCalibration();
#ifdef DEBUG
  Serial.println("CALIBRATION ROUTINE ENDED.");
#endif
}
#endif

#ifdef COCOJOYSTICK_PERSISTENCE
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
#ifdef DEBUG
    Serial.println("CoCoJoystick::saveCalibration()");
#endif
  if(_EEPROMOffset == -1) { return; };

  EEPROM.put(_EEPROMOffset, *_axisX.getInput());
  EEPROM.put(_EEPROMOffset + sizeof(mappingData_t), *_axisY.getInput());

  saveCRC();
#ifdef DEBUG
  Serial.println("Axis X:");
  _axisX.printCalibration();
  Serial.println("Axis Y:");
  _axisY.printCalibration();
#endif
  loadCalibration();
}

void CoCoJoystick::loadCalibration() {
#ifdef DEBUG
    Serial.println("CoCoJoystick::loadCalibration()");
#endif
  if(_EEPROMOffset == -1) { 
#ifdef DEBUG
    Serial.println("EEPROM Offset not set");
#endif
    _axisX.setDefault();
    _axisY.setDefault();
    return;
  };

  if(checkCRC()) {
#ifdef DEBUG
    Serial.println("Loading calibration from EEPROM");
#endif
    mappingData_t dataX;
    EEPROM.get(_EEPROMOffset, dataX);
    _axisY.setInput(dataX);
    
    mappingData_t dataY;
    EEPROM.get(_EEPROMOffset + sizeof(mappingData_t), dataY);
    _axisY.setInput(dataY);
  } else {
#ifdef DEBUG
    Serial.println("Loading default calibration");
#endif
    _axisX.setDefault();
    _axisY.setDefault();
  }

#ifdef DEBUG
  Serial.println("Axis X:");
  _axisX.printCalibration();
  Serial.println("Axis Y:");
  _axisY.printCalibration();
#endif
}
#endif
