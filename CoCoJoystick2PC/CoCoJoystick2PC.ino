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

#ifdef CALIBRATION
const int calibrateButtonPin = 8;
const int calibrateLedPin = LED_BUILTIN;
ButtonEvent calibrateButton;
#endif

const int EEPROMOffset = 0;

  // TODO: OINK!
//#include "SerialPrintCoCoJoystickEvent.h"
//#include "HIDProjectUSBCoCoJoystickEvent.h"
//#include "USBCoCoJoystickEvent.h"
//#include "SmallUSBCoCoJoystickEvent.h"
#include "USBSingleCoCoJoystickEvent.h"


const int joystick1PinX = A0; // brown
const int joystick1PinY = A1; // red
const int joystick1PinBTN_RED = 16; // yellow
const int joystick1PinBTN_BLACK = 14; // blue
#ifdef DETECT_JOYSTICK
const int joystick1PinShell = 10; // mesh / outer shell
#else
const int joystick1PinShell = -1;
#endif
// green => +5
// orange => gnd
CoCoJoystick joystick1;

#ifdef TWO_JOYSTICKS
#include "USBCoCo2JoystickEvent.h"
//#include "SerialPrintCoCoJoystickEvent.h"
const int joystick2PinX = A2; // red
const int joystick2PinY = A3; // blue
const int joystick2PinBTN_RED = 15; // yellow
const int joystick2PinBTN_BLACK = 14; // blue
// green 0v
// brown 5v
#ifdef DETECT_JOYSTICK
const int joystick2PinShell = 10; // mesh / outer shell
#else
const int joystick2PinShell = -1;
const int EEPROMOffset2 = 200;
#endif
// green => +5
// orange => gnd
CoCoJoystick joystick2;
#endif

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  while(!Serial) {} // for Leonardo
  Serial.println("::setup()");
#endif

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
  //joystick2.setConfig(new USBCoCo2JoystickEvent()); 
  //joystick2.setConfig(new SerialPrintCoCoJoystickEvent()); 
  joystick2.setConfig(new USBSingleCoCoJoystickEvent()); 
  
  joystick2.setup(joystick2PinX, joystick2PinY, joystick2PinBTN_RED, joystick2PinBTN_BLACK, joystick2PinShell, EEPROMOffset2);
#endif
  
#ifdef CALIBRATION
#ifdef DEBUG
  Serial.println("::calibrateButton.setup()");
#endif
  calibrateButton.setup(calibrateButtonPin);
#endif
}

#ifdef CALIBRATION
bool calibrateActuated = false;

void calibrateStart(uint32_t forMs, void *obj) {
#ifdef DEBUG
  Serial.println("::calibrateStart(...)");
#endif
  joystick1.startCalibration();
#ifdef TWO_JOYSTICKS
  joystick2.startCalibration();
#endif
}

void calibrateReset(void *obj) {
#ifdef DEBUG
  Serial.println("::calibrateStart(...)");
#endif
  joystick1.resetCalibration();
}
#endif

void loop() {
  uint32_t now = millis();

#ifdef CALIBRATION
  calibrateButton.loop(now);
  calibrateButton.onPressedFor(3000, calibrateReset, calibrateActuated);
  calibrateButton.onPressed(calibrateStart);
#endif

  joystick1.loop(now);
#ifdef TWO_JOYSTICKS
  joystick2.loop(now);
#endif
}
