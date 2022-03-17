/**
 * TODO:
 *   Setar o led para piscar na calibragem
 *   Fazer o debounce de verdade
 *   Fazer poder receber dois joysticks (colcoar a calibragem em um objeto)
 *   Rodar no arv
 */

#include <HID-Project.h>
#include <HID-Settings.h>

#include <HID.h>
#include <EEPROM.h>
#ifdef MY_LEONARDO
int pinX = A0;
int pinY = A1;
int pinBTN_RED = 12;
int pinBTN_BLACK = 11;
int pinCALIBRATE = 13;
#else
int pinX = A0;
int pinY = A1;
int pinBTN_RED = 10;
int pinBTN_BLACK = 11;
int pinCALIBRATE = 9;
#endif
int USB_BEGIN = -32767;
int USB_END = 32767;
int USB_CENTER = 0;
int USB_BUTTONS_RED[] = {1};
int USB_BUTTONS_BLACK[] = {2};
int BLINK_PIN = LED_BUILTIN;

static int prevX = 0;
static int prevY = 0;
static int prevBtnRed = 2;
static int prevBtnBlack = 2;

static void (*state)(void);

void debounce() {
  // Simple debounce
  delay(100);
}

// ************************************************** //Serial.print **************************************************
#define DEBUG YES
#ifdef DEBUG
void debug(String x) { Serial.print(x); } void debugln(String x) { Serial.println(x); }
void debug(long x) { Serial.print(x); } void debugln(long x) { Serial.println(x); }
#else
void debug(String x) { } void debugln(String x) { }
void debug(long x) { } void debugln(long x) { }
#endif

unsigned long eeprom_crc(char *data, int length) {

  const unsigned long crc_table[16] = {

    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,

    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,

    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,

    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c

  };

  unsigned long crc = ~0L;

  for (int index = 0 ; index < length  ; ++index) {

    crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);

    crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);

    crc = ~crc;

  }

  return crc;
}

// ************************************************** BLINK **************************************************
/**
 *  int pin => ping number to blink
 *  int times => number of repetitions
 *  String pattern => string with the patern. Syntax:
 *    UPPER CASE the led is on
 *    lower case the led is off
 *    
 *    s (short) waits 10 miliseconds
 *    l (long) waits 100 miliseconds
 *    v (very long) waits 1000 miliseconds
 */
int BLINK_SHORT = 10;
int BLINK_LONG = 100;
int BLINK_VERY_LONG = 1000;
void blinkLed(int pin = LED_BUILTIN, String pattern = "Ll", int times = 1 ) {
  for(int i = 0; i < times; i++) {
    for(int pos = 0; pos < pattern.length(); pos++) {
      switch(pattern[pos]) {
        case 's':
          digitalWrite(pin, LOW);
          delay(BLINK_SHORT);
          break;
        case 'l':
          digitalWrite(pin, LOW);
          delay(BLINK_LONG);
          break;
        case 'v':
          digitalWrite(pin, LOW);
          delay(BLINK_VERY_LONG);
          break;
        case 'S':
          digitalWrite(pin, HIGH);
          delay(BLINK_SHORT);
          break;
        case 'L':
          digitalWrite(pin, HIGH);
          delay(BLINK_LONG);
          break;
        case 'V':
          digitalWrite(pin, HIGH);
          delay(BLINK_VERY_LONG);
          break;
        default:
          // do nothing, ignore
          break;
      }
    }
  }
}


// ************************************************** CALIBRATION **************************************************
struct maxmin {
  int maximum;
  int minimum; 
};

struct axisCalibration {
  struct maxmin edge;
  struct maxmin center;
};

struct joystickCalibration {
  struct axisCalibration x;
  struct axisCalibration y;
};

struct joystickCalibration calibration;

struct joystickCalibration temp_calibration;

void calibrate_print(struct joystickCalibration *jc) {
  debugln("Calibration: ");
  debug("  X => edge: ["); debug(jc->x.edge.minimum); debug(", "); debug(jc->x.edge.maximum); debugln("]");
  debug("       center: ["); debug(jc->x.center.minimum); debug(", "); debug(jc->x.center.maximum); debugln("]");
  debug("  Y => edge: ["); debug(jc->y.edge.minimum); debug(", "); debug(jc->y.edge.maximum); debugln("]");
  debug("       center: ["); debug(jc->y.center.minimum); debug(", "); debug(jc->y.center.maximum); debugln("]");
}

void calibrate_start() {
    //blinkLed(LED_BUILTIN, "SsSs", 10);
    debugln("*** CALIBRATION START");
    
    temp_calibration.x.edge.minimum = 1023;
    temp_calibration.x.edge.maximum = 0;
    temp_calibration.y.edge.minimum = 1023;
    temp_calibration.y.edge.maximum = 0;

    temp_calibration.x.center.minimum = 1023;
    temp_calibration.x.center.maximum = 0;
    temp_calibration.y.center.minimum = 1023;
    temp_calibration.y.center.maximum = 0;

    state = &calibrate_edge;
}

void calibrate_accumulate(struct maxmin *mm, int rawValue) {
  mm->maximum = mm->maximum < rawValue ? rawValue : mm->maximum;
  mm->minimum = mm->minimum > rawValue ? rawValue : mm->minimum;
}

void calibrate_edge() {
  int rawValueX = analogRead(pinX);
  int rawValueY = analogRead(pinY);

  calibrate_accumulate(&(temp_calibration.x.edge), rawValueX);
  calibrate_accumulate(&(temp_calibration.y.edge), rawValueY);
  
  debounce();

  if(!digitalRead(pinBTN_RED) || !digitalRead(pinBTN_BLACK)) {
    debugln("*** CALIBRATION EDGE ENDED");
    //Serial.print("Edge: MaxX = " ); //Serial.print(temp_calibration.x.edge.maximum);
    //Serial.print(" MinX = " ); //Serial.print(temp_calibration.x.edge.minimum);
    //Serial.print(" MaxY = " ); //Serial.print(temp_calibration.y.edge.maximum);
    //Serial.print(" MinY = " ); //Serial.println(temp_calibration.y.edge.minimum);
    //blinkLed(BLINK_PIN, "Ll", 2);
    delay(1000);
    state = &calibrate_center;
  }
}


void calibrate_center() {
  int rawValueX = analogRead(pinX);
  int rawValueY = analogRead(pinY);

  calibrate_accumulate(&(temp_calibration.x.center), rawValueX);
  calibrate_accumulate(&(temp_calibration.y.center), rawValueY);

  debounce();

  if(!digitalRead(pinBTN_RED) || !digitalRead(pinBTN_BLACK)) {
    debugln("*** CALIBRATION CENTER ENDED");
    delay(1000);
    state = &calibrate_end;
  }
}


void calibrate_end() {
    debug("*** CALIBRATION END");
    calibrate_save(&temp_calibration); /* */
    calibrate_load();
    state = &operation;
}

String SIGNATURE="CCJY";

int SAVE_ADDRESS=0;
int SAVE_ADDRESS_DATA=SAVE_ADDRESS + sizeof(SIGNATURE);
int SAVE_ADDRESS_CRC=SAVE_ADDRESS_DATA + sizeof(struct joystickCalibration);

void calibrate_save(struct joystickCalibration *jc) {
  debugln("*** EEPROM SAVE");
  calibrate_print(jc);
  EEPROM.put(SAVE_ADDRESS, SIGNATURE);
  EEPROM.put(SAVE_ADDRESS_DATA, *jc);
  unsigned long crc = eeprom_crc((char *)jc, sizeof(struct joystickCalibration));
  EEPROM.put(SAVE_ADDRESS_CRC, crc);
}

void calibrate_load() {
  debugln("*** EEPROM LOAD");
  debugln("    Before:");
  calibrate_print(&calibration);
  String signature = "XXXX";
  EEPROM.get(SAVE_ADDRESS, signature);
  EEPROM.get(SAVE_ADDRESS_DATA, temp_calibration);
  unsigned long crcCalc = eeprom_crc((char *)(&temp_calibration), sizeof(struct joystickCalibration));
  unsigned long crc = 0;
  EEPROM.get(SAVE_ADDRESS_CRC, crc);

  int isOK = 1;
  //Serial.print("SIGNATURE: "); //Serial.print(signature);
  if(signature == SIGNATURE) {
    //Serial.println(" OK");
  } else {
    debug("SIGNATURE MISMATCH");
    isOK = 0;
  }
  //Serial.print("CRC: "); //Serial.print(crc);  //Serial.print(" =? "); //Serial.print(crcCalc); 
  if(crc == crcCalc) {
    //Serial.println("CRC OK");
  } else {
    debug("CRC MISMATCH");
    isOK = 0;
  }

  if(isOK) {
    debugln("LOAD EEPROM");
    calibration.x.edge.minimum = temp_calibration.x.edge.minimum;
    calibration.x.edge.maximum = temp_calibration.x.edge.maximum;
    calibration.x.center.minimum = temp_calibration.x.center.minimum;
    calibration.x.center.maximum = temp_calibration.x.center.maximum;
    
    calibration.y.edge.minimum = temp_calibration.y.edge.minimum;
    calibration.y.edge.maximum = temp_calibration.y.edge.maximum;
    calibration.y.center.minimum = temp_calibration.y.center.minimum;
    calibration.y.center.maximum = temp_calibration.y.center.maximum;
  } else {
    debugln("LOAD DEFAULT");
    calibration.x.edge.minimum = 0;
    calibration.x.edge.maximum = 1023;
    calibration.x.center.minimum = 479;
    calibration.x.center.maximum = 543;
    
    calibration.y.edge.minimum = 0;
    calibration.y.edge.maximum = 1023;
    calibration.y.center.minimum = 479;
    calibration.y.center.maximum = 543;
  }
  debugln("    After:");
  calibrate_print(&calibration);
}

void calibrate_init() {
  calibrate_load();
}

int calibrate_map(int rawValue, struct axisCalibration *axis) {
  // return map(rawValueX, 0, 1023, USB_BEGIN, USB_END);
  if(rawValue >= axis->center.minimum && rawValue <= axis->center.maximum) return USB_CENTER;
  int center = (axis->center.maximum + axis->center.minimum)/2;
  if(rawValue < axis->center.minimum) return map(rawValue, axis->edge.minimum, center, USB_BEGIN, USB_CENTER);
  /* else */
  return map(rawValue, center, axis->edge.maximum, USB_CENTER, USB_END);
}

// ************************************************** OPERATION **************************************************

void operation() {
  // put your main code here, to run repeatedly:
  int rawValueX = analogRead(pinX);
  int valueX = calibrate_map(rawValueX, &(calibration.x));
  Gamepad1.xAxis(valueX);

  int rawValueY = analogRead(pinY);
  int valueY = calibrate_map(rawValueY, &(calibration.y));
  Gamepad1.yAxis(valueY);

  int valueBtnRed = !digitalRead(pinBTN_RED);
  if(valueBtnRed) {
    debug("R");
    for (int i = 0; i < (sizeof(USB_BUTTONS_RED) / sizeof(USB_BUTTONS_RED[0])); i++) {
      Gamepad1.press(USB_BUTTONS_RED[i]);
    }
  } else {
    for (int i = 0; i < (sizeof(USB_BUTTONS_RED) / sizeof(USB_BUTTONS_RED[0])); i++) {
      Gamepad1.release(USB_BUTTONS_RED[i]);
    }
  }

  int valueBtnBlack = !digitalRead(pinBTN_BLACK);
  if(valueBtnBlack) {
    debug("B");
    for (int i = 0; i < (sizeof(USB_BUTTONS_BLACK) / sizeof(USB_BUTTONS_BLACK[0])); i++) {
      Gamepad1.press(USB_BUTTONS_BLACK[i]);
    }
  } else {
    for (int i = 0; i < (sizeof(USB_BUTTONS_BLACK) / sizeof(USB_BUTTONS_BLACK[0])); i++) {
      Gamepad1.release(USB_BUTTONS_BLACK[i]);
    }
  }

  // Functions above only set the values.
  // This writes the report to the host.
  if(prevX != valueX || prevY != valueY || prevBtnRed != valueBtnRed || prevBtnBlack != valueBtnBlack) {
    Gamepad1.write();
    prevX = valueX;
    prevY = valueY;
    prevBtnRed = valueBtnRed;
    prevBtnBlack = valueBtnBlack;

    //Serial.print("X = ");
    //Serial.print(valueX);
    //Serial.print("Y = ");
    //Serial.print(valueY);
    if(valueBtnRed) {
      //Serial.print("[X]");
    } else {
      //Serial.print("[ ]");
    }
    if(valueBtnBlack) {
      //Serial.println("[X]");
    } else {
      //Serial.println("[ ]");
    }
  }
  
  debounce();
}

void setup() {
    // put your setup code here, to run once:
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinBTN_RED, INPUT_PULLUP);
  pinMode(pinBTN_BLACK, INPUT_PULLUP);
  pinMode(pinCALIBRATE, INPUT_PULLUP);

  calibrate_init();
  // Sends a clean report to the host. This is important on any Arduino type.
  //Gamepad1.begin();
  Gamepad1.begin();
  Gamepad2.begin();
  state = &operation;
}

void loop() {
  state();

  if(!digitalRead(pinCALIBRATE)) {
    state = &calibrate_start;
  }
}
