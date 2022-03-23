#include "AxisEvent.h"

#ifdef CALIBRATION
#include <EEPROM.h>
#endif

AxisEvent::AxisEvent() {
  _pin = -1;
#ifdef CALIBRATION
  _state = OPERATING;
#endif
}

void AxisEvent::setup(int pin, int tolerance = 0) {
  _pin = pin;
  _tolerance = tolerance;
  
  setOutput(defaultOutputMinimum, defaultOutputMaximum, defaultOutputCenter);
  setDefault();

  pinMode(pin, INPUT);
}

void AxisEvent::setOutput(int minimum, int maximum, int center) {
  mapping.outputMinimum = minimum;
  mapping.outputMaximum = maximum;
  mapping.outputCenter = center;
}

void AxisEvent::setDefault() {
  mapping.input.edgeMinimum = defaultInputMinimum;
  mapping.input.edgeMaximum = defaultInputMaximum;
  mapping.input.centerMinimum = defaultInputCenterMinium;
  mapping.input.centerMaximum = defaultInputCenterMaximum;
}

void AxisEvent::onChanged(void(*callback)(int, void *), void *obj = (void *)0, uint32_t now = millis()) {
  if(_pin == -1) return; // must use setup() first

  int rawValue = analogRead(_pin);

#ifdef CALIBRATION
  if(_state == STATE::CALIBRATING_EDGES) {
    tempCalibration.edgeMinimum = min(tempCalibration.edgeMinimum, rawValue);
    tempCalibration.edgeMaximum = max(tempCalibration.edgeMaximum, rawValue);
  }
  
  if(_state == STATE::CALIBRATING_CENTERS) {
    tempCalibration.centerMinimum = min(tempCalibration.centerMinimum, rawValue);
    tempCalibration.centerMaximum = max(tempCalibration.centerMaximum, rawValue);
  }
#endif

  if(_lastValue - rawValue > _tolerance || rawValue - _lastValue > _tolerance) {
    _lastValue = rawValue;
    callback(value(), obj); 
  }
}

#ifdef CALIBRATION

void AxisEvent::startCalibration() {
  _state = STATE::CALIBRATING_EDGES;
  tempCalibration.edgeMinimum = defaultInputMaximum;
  tempCalibration.edgeMaximum = defaultInputMinimum;
  tempCalibration.centerMinimum = defaultInputMaximum;
  tempCalibration.centerMaximum = defaultInputMinimum;
}

void AxisEvent::centerCalibration() {
  _state = STATE::CALIBRATING_CENTERS;
}

void AxisEvent::endCalibration() {
  _state = STATE::OPERATING;
  mapping.input.edgeMinimum = tempCalibration.edgeMinimum;
  mapping.input.edgeMaximum = tempCalibration.edgeMaximum;
  mapping.input.centerMinimum = tempCalibration.centerMinimum;
  mapping.input.centerMaximum = tempCalibration.centerMaximum;
}

void AxisEvent::cancelCalibration() {
  _state = STATE::OPERATING;
}

void AxisEvent::resetCalibration() {
  cancelCalibration();
  setDefault();
}

void AxisEvent::printCalibration() {
  mapping.print();
}
#endif
