/**
 * DIN-6 Layout
 *    [female]      1 => Wiper X
 *       U          2 => Wiper Y
 *    5     1       3 => Ground
 *       6          4 => Fire 1
 *    4     2       5 => +5 Pots X & Y
 *       3          6 => Fire 2
 */
#include "config.h"

#include "CoCoJoystick.h"
#include "ButtonEvent.h"
#include "Blinker.h"
#include "StateButtonEvent.h"

//#define TWO_JOYSTICKS
//#define ENABLE_INVERT_JOYSTICKS
//#define DEBUG
#define DETECT_JOYSTICK

Blinker led;

const int calibrateButtonPin = 8;
const int calibrateLedPin = LED_BUILTIN;
ButtonEvent calibrateButton;
const int EEPROMOffset = 0;

  // TODO: OINK!
//#include "SerialPrintCoCoJoystickEvent.h"
//#include "HIDProjectUSBCoCoJoystickEvent.h"
#include "USBCoCoJoystickEvent.h"
//#include "SmallUSBCoCoJoystickEvent.h"
//#include "USBSingleCoCoJoystickEvent.h"

const int joystick1PinX = A0; // DIN-6 Pin 1 - brown 
const int joystick1PinY = A1; // DIN-6 Pin 2 - red
// GND => DIN-6 Pin 3 - orange
const int joystick1PinBTN_RED = 9; // DIN-6 Pin 4 - yellow
// +5 => DIN-6 Pin 5 - green
const int joystick1PinBTN_BLACK = 10; // DIN-6 Pin 6 - blue
#ifdef DETECT_JOYSTICK
const int joystick1PinShell = 11; // mesh / outer shell
#else
const int joystick1PinShell = -1;
#endif
CoCoJoystick joystick1;

#ifdef TWO_JOYSTICKS
#include "USBCoCo2JoystickEvent.h"
//#include "SerialPrintCoCoJoystickEvent.h"
const int joystick2PinX = A2; // DIN-6 Pin 1 - red
const int joystick2PinY = A3; // DIN-6 Pin 2 - blue
// GND => DIN-6 Pin 3 - green
const int joystick2PinBTN_RED = 15; // DIN-6 Pin 2 - yellow
// +5 => DIN-6 Pin 5 - brown
const int joystick2PinBTN_BLACK = 14; // DIN-6 Pin 2 - blue
#ifdef DETECT_JOYSTICK
const int joystick2PinShell = 10; // mesh / outer shell
#else
const int joystick2PinShell = -1;
const int EEPROMOffset2 = 200;
#endif
CoCoJoystick joystick2;

#ifdef ENABLE_INVERT_JOYSTICKS
const int invertJoysticksPin = 9;
StateButtonEvent invertJoystickButton(2);
#endif

#endif

  // use red button for 10 seconds
#define CALIBRATE 1
  // use spare button
//#define CALIBRATE 2

#if CALIBRATE == 1
unsigned long startTime;
#endif

void setup() {
#if CALIBRATE == 1
  startTime = millis();
#endif

  led.setup(calibrateLedPin);
  led.setFade(32, 10);
  
  // TODO: OINK!
#ifdef SerialPrintCoCoJoystickEvent_h
  joystick1.setConfig(new SerialPrintCoCoJoystickEvent());
#endif
#ifdef HIDProjectUSBCoCoJoystickEvent_h
  joystick1.setConfig(new HIDProjectUSBCoCoJoystickEvent());
#endif
#ifdef USBCoCoJoystickEvent_h
  joystick1.setConfig(new USBCoCoJoystickEvent()); 
#endif
#ifdef SmallUSBCoCoJoystickEvent_h
  joystick1.setConfig(new SmallUSBCoCoJoystickEvent()); 
#endif
#ifdef USBSingleCoCoJoystickEvent_h
  joystick1.setConfig(new USBSingleCoCoJoystickEvent()); 
#endif

  joystick1.setup(joystick1PinX, joystick1PinY, joystick1PinBTN_RED, joystick1PinBTN_BLACK, joystick1PinShell, EEPROMOffset);

#ifdef TWO_JOYSTICKS
  joystick2.setConfig(new USBCoCo2JoystickEvent()); 
  //joystick2.setConfig(new SerialPrintCoCoJoystickEvent()); 
  //joystick2.setConfig(new USBSingleCoCoJoystickEvent()); 
  
  joystick2.setup(joystick2PinX, joystick2PinY, joystick2PinBTN_RED, joystick2PinBTN_BLACK, joystick2PinShell, EEPROMOffset2);
#endif

  debugStart();

#if CALIBRATE == 1
  calibrateButton.setup(joystick1PinBTN_RED);
  led.play(shortBliks);
#endif
#if CALIBRATE == 2
  calibrateButton.setup(calibrateButtonPin);
#endif

#ifdef ENABLE_INVERT_JOYSTICKS
  invertJoystickButton.setup(invertJoysticksPin);
#endif
}

bool calibrateActuated = false;

void calibrateStart(uint32_t forMs, void *obj) {
  debugfunction("::calibrateStart(...)");
  delay(1000);
  joystick1.startCalibration();
#ifdef TWO_JOYSTICKS
  joystick2.startCalibration();
#endif
}

void calibrateReset(void *obj) {
  debugfunction("::calibrateReset(...)");
  joystick1.resetCalibration();
}

#ifdef ENABLE_INVERT_JOYSTICKS
void invertJoysticks(state_t state, void *obj) {
  debugfunction("::invertJoysticks(...)");
  CoCoJoystickEvent *temp;
  temp = joystick1.getConfig();
  joystick1.setConfig(joystick2.getConfig());
  joystick2.setConfig(temp);
  debugln("invertJoysticks(...)");
}
#endif

#if CALIBRATE == 1
void calibrateCenter(CoCoJoystick *obj) {
  Serial.println("CENTER");
  led.play(twiceBliks);
}
void calibrateFinish(CoCoJoystick *obj) {
  Serial.println("FINISH");
  calibrateActuated = true;
  led.playOff();
}

void calibrateStart2(void *obj) {
  led.play(onceBliks);
  debugfunction("::calibrateStart(...)");
  delay(200);
  Serial.println("BEGIN");
  joystick1.startCalibration(calibrateCenter, calibrateFinish);
#ifdef TWO_JOYSTICKS
  joystick2.startCalibration();
#endif
}
void calibrateTest(uint32_t ms, void *obj) {
  Serial.println("TEST");
  return;
}
#define CALIBRATE_FIRST_SECS 60
#endif

void loop() {
  uint32_t now = millis();

  led.loop(now);
  calibrateButton.loop(now);
#if CALIBRATE == 1
  debugvar("calibrateActuated = ", calibrateActuated); debugln("");
  if(!calibrateActuated)  
  { 
    if(now < startTime + CALIBRATE_FIRST_SECS * 1000l) { // first 60 seconds
        calibrateButton.onPressedFor(5000, calibrateStart2, calibrateActuated);
        //calibrateButton.onPressed(calibrateTest);
    } else { // after CALIBRATE_FIRST_SECS seconds, can't calibrate anymore
      calibrateActuated = true;
      led.playOff();
    }
  }
  
#endif
#if CALIBRATE == 2
    calibrateButton.onPressedFor(10000, calibrateReset, calibrateActuated);
    calibrateButton.onPressed(calibrateStart);
#endif

#ifdef ENABLE_INVERT_JOYSTICKS
  invertJoystickButton.loop(now);
  invertJoystickButton.onStateChange(invertJoysticks);
#endif

  joystick1.loop(now);
#ifdef TWO_JOYSTICKS
  joystick2.loop(now);
#endif
}
