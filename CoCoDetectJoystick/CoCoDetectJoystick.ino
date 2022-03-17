/**
 * Color Computer (CoCo) Joystick detector - prototype
 * 
 * Detects if the joystick is connected. The joystic circuit is 100k two potentiometers (extremities on +5 and GND, the middle connector is the output), and two buttons, each one linked to ground in one end.
 * 
 * The detection is made using the VCC pin and INPUT_PULLUP
 * 
 * Just connect it to pin 9 (or change joystick1PinVCC).
 * 
 * If you define PROBE_SIGNAL, the program will save the first 500 outputs to see what is happening on the probe (with poorman's osciloscope: save the pin output 500 times in a loop, then show the transitions on serial port)
 * 
 */

const int joystick1PinVCC = 9;
const int joystick1PinX = A0;
const int joystick1PinY = A1;
const int joystick1PinBTN_RED = 12;
const int joystick1PinBTN_BLACK = 11;

bool hasJoystick;

//#define PROBE_SIGNAL

void printStatus() {
  if(hasJoystick) {
    Serial.println("Has joystick");
  } else {
    Serial.println("No joystick");
  }
}

#ifdef PROBE_SIGNAL
const int osciloscopeMaxSize = 512;
int osciloscope[osciloscopeMaxSize];
int osciloscopeSize = 0;
void osciloscopeMeasure(int pin, int size) {
  if(size > osciloscopeMaxSize) size = osciloscopeMaxSize;

  for(int i = 0; i < size; i++) {
    osciloscope[i] = digitalRead(pin);
  }
  osciloscopeSize = size;
}

void osciloscopePrint() {
  int last;
  Serial.print(0);
  Serial.print(": ");
  Serial.println(osciloscope[0]);
  last = osciloscope[0];
  for(int i = 1; i < osciloscopeSize; i++) {
    if(osciloscope[i] != last) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(osciloscope[i]);
      last = osciloscope[i];
    }
  }
}
#endif

bool detectJoystick() {
  pinMode(joystick1PinVCC, OUTPUT);
  digitalWrite(joystick1PinVCC, false);
  
  int ip;
  pinMode(joystick1PinVCC, INPUT_PULLUP);
#ifdef PROBE_SIGNAL
  osciloscopeMeasure(joystick1PinVCC, 512);
  ip = osciloscope[0]; // first measure
#else
  ip = digitalRead(joystick1PinVCC);
#endif

  pinMode(joystick1PinVCC, OUTPUT);
  digitalWrite(joystick1PinVCC, true);
  
  return !ip;
}

void setup() {
  Serial.begin(9600);

  pinMode(joystick1PinX, INPUT);
  pinMode(joystick1PinY, INPUT);
  pinMode(joystick1PinBTN_RED , INPUT_PULLUP);
  pinMode(joystick1PinBTN_BLACK, INPUT_PULLUP);

  Serial.println("Start");
  hasJoystick = detectJoystick();
  printStatus();
}

void loop() {
  bool state = detectJoystick();
#ifdef PROBE_SIGNAL
  osciloscopePrint();
#endif
  if(state != hasJoystick) {
    hasJoystick = state;
    printStatus();
    
  }
}
