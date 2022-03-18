#include "AxisEvent.h"

AxisEvent::AxisEvent() {
  _pin = -1;
  _state = OPERATING;
}

void AxisEvent::setup(int pin, int EEPROMOffset, int tolerance = 0) {
  _pin = pin;
  _tolerance = tolerance;
  calibration.setEEPROMOffset(EEPROMOffset);
  calibration.loadEEPROM();
  //calibration.setOutput(DEFAULT_MINIMUM, DEFAULT_CENTER, DEFAULT_MAXIMUM);

  pinMode(pin, INPUT);
}

void AxisEvent::onChanged(void(*callback)(int, void *), void *obj = (void *)0, uint32_t now = millis()) {
  if(_pin == -1) return; // must use setup() first

  int value = analogRead(_pin);

  if(_state == STATE::CALIBRATING_EDGES) {
    _temporaryCalibration._edge.minimum = min(_temporaryCalibration._edge.minimum, value);
    _temporaryCalibration._edge.maximum = max(_temporaryCalibration._edge.maximum, value);
  }
  
  if(_state == STATE::CALIBRATING_CENTERS) {
    _temporaryCalibration._center.minimum = min(_temporaryCalibration._center.minimum, value);
    _temporaryCalibration._center.maximum = max(_temporaryCalibration._center.maximum, value);
  }

  if(_lastValue - value > _tolerance || value - _lastValue > _tolerance) {
    _lastValue = value;
    callback(calibration.map(value), obj); 
  }
}

void AxisEvent::setDefaultCalibration() {
    calibration.setEdge(defaultMinimum, defaultMaximum);
    calibration.setCenter(defaultCenterMinium , defaultCenterMaximum);
}

void AxisEvent::loadCalibration() {
    calibration.loadEEPROM();
}

void AxisEvent::saveCalibration() {
    calibration.saveEEPROM();
}

void AxisEvent::startCalibration() {
  _state = STATE::CALIBRATING_EDGES;
  _temporaryCalibration._edge.minimum = defaultMaximum;
  _temporaryCalibration._edge.maximum = defaultMinimum;
  _temporaryCalibration._center.minimum = defaultMaximum;
  _temporaryCalibration._center.maximum = defaultMinimum;
}

void AxisEvent::centerCalibration() {
  _state = STATE::CALIBRATING_CENTERS;
}

void AxisEvent::endCalibration() {
  _state = STATE::OPERATING;
  calibration.setEdge(_temporaryCalibration._edge.minimum, _temporaryCalibration._edge.maximum);
  calibration.setCenter(_temporaryCalibration._center.minimum, _temporaryCalibration._center.maximum);
}

void AxisEvent::printCalibration() {
  calibration.printOutput();
  calibration.printCalibration();
}
