/**
 * DIN-6 Layout
 *    [female]      1 => Wiper X
 *       U          2 => Wiper Y
 *    5     1       3 => Ground
 *       6          4 => Fire 1
 *    4     2       5 => +5 Pots X & Y
 *       3          6 => Fire 2
 */
#include "CoCoJoystick.h"
#include "ButtonEvent.h"

#define DEBUG

const int joystick1PinX = A0; // brown
const int joystick1PinY = A1; // red
const int joystick1PinBTN_RED = 12; // yellow
const int joystick1PinBTN_BLACK = 11; // blue
const int joystick1PinShell = 10; // mesh / outer shell
// green => +5
// orange => gnd
const int calibrateButtonPin = 8;

const int EEPROMOffset = 0;

ButtonEvent calibrateButton;

CoCoJoystick joystick1;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  //while(!Serial) {} // for Leonardo
#endif
  joystick1.setup(joystick1PinX, joystick1PinY, joystick1PinBTN_RED, joystick1PinBTN_BLACK, joystick1PinShell, EEPROMOffset);
  calibrateButton.setup(calibrateButtonPin);
}

void calibrateStart(uint32_t forMs, void *obj) {
  joystick1.startCalibration();
}

void loop() {
  uint32_t now = millis();
  
  calibrateButton.loop(now);
  calibrateButton.onPressed(calibrateStart);

  joystick1.loop(now);
}
