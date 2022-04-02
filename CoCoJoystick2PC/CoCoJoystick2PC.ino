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

#ifdef CALIBRATION
const int calibrateButtonPin = 8;
const int calibrateLedPin = LED_BUILTIN;
ButtonEvent calibrateButton;
#endif

const int EEPROMOffset = 0;

CoCoJoystick joystick1;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  while(!Serial) {} // for Leonardo
  Serial.println("::setup()");
#endif

  joystick1.setup(joystick1PinX, joystick1PinY, joystick1PinBTN_RED, joystick1PinBTN_BLACK, joystick1PinShell, EEPROMOffset);
  
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
}
