#include <HID-Project.h>
#include <HID-Settings.h>

#include <HID.h>

int pinX = A0;
int pinY = A1;
int pinBTN0 = 11;
int begin = -32767;
int end = 32767;
int BUTTON = 1;

void setup() {
    // put your setup code here, to run once:
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinBTN0, INPUT_PULLUP);

  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

static int prevX = 0;
static int prevY = 0;
static int prevBtn = 2;

void loop() {
  // put your main code here, to run repeatedly:
  int rawValueX = analogRead(pinX);
  int valueX = map(rawValueX, 0, 1023, begin, end);
  int rawValueY = analogRead(pinY);
  int valueY = map(rawValueY, 0, 1023, begin, end);

  int valueBtn = !digitalRead(pinBTN0);
  if(valueBtn) {
    Gamepad.press(BUTTON);
  } else {
    Gamepad.release(BUTTON);
  }

  Gamepad.xAxis(valueX);
  Gamepad.yAxis(valueY);

  // Functions above only set the values.
  // This writes the report to the host.
  if(prevX != valueX || prevY != valueY || prevBtn != valueBtn) {
    Gamepad.write();
    prevX = valueX;
    prevY = valueY;
    prevBtn = valueBtn;

    Serial.print("X = ");
    Serial.print(valueX);
    Serial.print("Y = ");
    Serial.print(valueY);
    if(valueBtn) {
      Serial.println("[X]");
    } else {
      Serial.println("[ ]");
    }
  }
  
  // Simple debounce
  delay(100);
}
