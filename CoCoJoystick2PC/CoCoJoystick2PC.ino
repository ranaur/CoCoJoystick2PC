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
#define TWO_JOYSTICKS

#include "CoCoJoystick.h"
#include "ButtonEvent.h"

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
const int joystick1PinBTN_RED = 16; // DIN-6 Pin 4 - yellow
// +5 => DIN-6 Pin 5 - green
const int joystick1PinBTN_BLACK = 14; // DIN-6 Pin 6 - blue
#ifdef DETECT_JOYSTICK
const int joystick1PinShell = 10; // mesh / outer shell
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
#endif

void setup() {
  
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
  
  //debugStart();
  while(!Serial) {}
  debugfunction("::setup()");

  debugfunction("::calibrateButton.setup()");
  calibrateButton.setup(calibrateButtonPin);
}

bool calibrateActuated = false;

void calibrateStart(uint32_t forMs, void *obj) {
  debugln("::calibrateStart(...)");
  joystick1.startCalibration();
#ifdef TWO_JOYSTICKS
  joystick2.startCalibration();
#endif
}

void calibrateReset(void *obj) {
  debugln("::calibrateStart(...)");
  joystick1.resetCalibration();
}

void loop() {
  uint32_t now = millis();

  calibrateButton.loop(now);
  calibrateButton.onPressedFor(10000, calibrateReset, calibrateActuated);
  calibrateButton.onPressed(calibrateStart);

  joystick1.loop(now);
#ifdef TWO_JOYSTICKS
  joystick2.loop(now);
#endif
}
