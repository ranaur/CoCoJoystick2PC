/*
Copyright (c) 2014-2015 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// Include guard
#pragma once

#include <Arduino.h>
#include "HID.h"
#include "HID-Settings.h"

typedef union ATTRIBUTE_PACKED {
  // 32 Buttons, 6 Axis, 2 D-Pads
  uint8_t whole8[0];
  uint16_t whole16[0];
  uint32_t whole32[0];
  uint8_t buttons;

  struct ATTRIBUTE_PACKED {
    uint8_t button1 : 1;
    uint8_t button2 : 1;
    uint8_t button3 : 1;
    uint8_t button4 : 1;
    uint8_t button5 : 1;
    uint8_t button6 : 1;
    uint8_t button7 : 1;
    uint8_t button8 : 1;

    int16_t xAxis;
    int16_t yAxis;
  };
} HID_GamepadReport_Data_t;


void printByte(uint8_t n);

class HIDGamepad_
{

public:
  HIDGamepad_(void);

  // GamepadAPI
  inline void begin(void) { end(); };
  inline void end(void) { releaseAll(); write(); };
  inline void write(void) { SendReport(&_report, sizeof(_report)); };
  inline void pressRed() { _report.button1 = 1; };
  inline void pressBlack() { _report.button2 = 1; };
  inline void releaseRed() { _report.button1 = 0; };
  inline void releaseBlack() { _report.button2 = 0; };
  inline void releaseAll(void) { memset(&_report, 0x00, sizeof(_report)); };

  inline void xAxis(int16_t a) { _report.xAxis = a; };
  inline void yAxis(int16_t a) { _report.yAxis = a; };

protected: 
    virtual inline void SendReport(void* data, int length) { 
#ifdef DEBUG
      Serial.print("REPORT [");
      Serial.print(length, HEX);
      Serial.print("]: ");
      for(int i = 0; i < length; i++) {
        Serial.print(" ");
        printByte(((uint8_t *)data)[i]);
      }
      Serial.println("");
#endif
      HID().SendReport(HID_REPORTID_GAMEPAD, data, length);
      };
    HID_GamepadReport_Data_t _report;

};
extern HIDGamepad_ HIDGamepad;
