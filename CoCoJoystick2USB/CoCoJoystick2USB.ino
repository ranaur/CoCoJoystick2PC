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

const int joystick1PinX = A0; // brown
const int joystick1PinY = A1; // red
const int joystick1PinBTN_RED = 12; // yellow
const int joystick1PinBTN_BLACK = 11; // blue
// orange => gnd
// green => +5
const int calibrateButtonPin = 8;

const int EEPROMOffset = 0;

ButtonEvent calibrateButton;

CoCoJoystick joystick1;

void setup() {
  Serial.begin(9600);
  //while(!Serial) {}
  joystick1.setup(joystick1PinX, joystick1PinY, joystick1PinBTN_RED, joystick1PinBTN_BLACK, EEPROMOffset);
  calibrateButton.setup(calibrateButtonPin);
}

void calibrateStart(uint32_t forMs, void *obj) {
  Serial.println("SO ... IT BEGINS");
  joystick1.startCalibration();
}

void loop() {
  uint32_t now = millis();
  
  calibrateButton.loop(now);
  //calibrateButton.printState();
  calibrateButton.onPressed(calibrateStart);

  joystick1.loop(now);
}

/**
 * TODO:
 * Permitir resetar totalmente se pressionar por 10 segundos.
 * Colocar o segundo Joystick
 * Criar USBGamepadConfigs:
 *    Red => 1 Black => 2 X => AxisX Y => AxisY
 *    Red => 1 Black => 1 X => AxisX Y => AxisY
 *    Red => 12346 Black => 12346 X => AxisX Y => AxisY
 *    Red => 1 Black => 2 X => RightAxisX => RightAxisY
 *    Red => 1 Black => 2 X => LeftAxisX => LeftAxisY
 *        (idem para o 11)
 */
