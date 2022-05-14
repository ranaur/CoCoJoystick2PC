#ifndef USBCoCoJoystickEvent_h
#define USBCoCoJoystickEvent_h

  // Show on serial the report data
#undef DEBUG_REPORT

#define DEFAULT_HID_REPORTID 6

#ifdef DEBUG_REPORT
#include "util.h"
#endif

#include "CoCoJoystickEvent.h"

#include "HID.h"

static const uint8_t _hidMultiReportDescriptorCoCoJoystick[] PROGMEM = {
  /* Gamepad with 2 buttons and 6 axis*/
  0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
  0x09, 0x04,             /* USAGE (Joystick) */
  0xa1, 0x01,             /* COLLECTION (Application) */
  0x85, DEFAULT_HID_REPORTID,     /*   REPORT_ID  */
  /* 2 Buttons */
  0x05, 0x09,             /*   USAGE_PAGE (Button) */
  0x19, 0x01,             /*   USAGE_MINIMUM (Button 1) */
  0x29, 0x02,             /*   USAGE_MAXIMUM (Button 2) */
  0x15, 0x00,             /*   LOGICAL_MINIMUM (0) */
  0x25, 0x01,             /*   LOGICAL_MAXIMUM (1) */
  0x75, 0x01,             /*   REPORT_SIZE (1) */
  0x95, 0x02,             /*   REPORT_COUNT (2) */
  0x81, 0x02,             /*   INPUT (Data,Var,Abs) */
  0x95, 0x06,             /*   REPORT_COUNT (6) PADDING */
  0x81, 0x03,             /*   INPUT (Constant,Var,Abs) */
  /* 2 16bit Axis */
  0x05, 0x01,             /*   USAGE_PAGE (Generic Desktop) */
  0xa1, 0x00,             /*   COLLECTION (Physical) */
  0x09, 0x30,             /*     USAGE (X) */
  0x09, 0x31,             /*     USAGE (Y) */
  0x16, 0x00, 0x80,       /*     LOGICAL_MINIMUM (-32768) */
  0x26, 0xFF, 0x7F,       /*     LOGICAL_MAXIMUM (32767) */
  0x75, 0x10,             /*     REPORT_SIZE (16) */
  0x95, 0x02,             /*     REPORT_COUNT (2) */
  0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
  0xc0,                   /*   END_COLLECTION */
  0xc0                    /* END_COLLECTION */
};

typedef struct {
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
} HID_CoCoJoystickReport_Data_t;

class USBCoCoJoystickEvent : public CoCoJoystickEvent {
  private:
  int _buttonRed;
  int _buttonBlack;
  bool changed;
    
  public:

  USBCoCoJoystickEvent() {
    static HIDSubDescriptor node(_hidMultiReportDescriptorCoCoJoystick, sizeof(_hidMultiReportDescriptorCoCoJoystick));
    HID().AppendDescriptor(&node);
    _report.button1 = 0;
    _report.button2 = 0;
    _report.xAxis = 0;
    _report.yAxis = 0;
    changed = true; // send first report on first loop call
  };
  
  void setButtonNumbers(int buttonRed, int buttonBlack ) {
    _buttonRed = buttonRed;
    _buttonBlack = buttonBlack;
  }
  
  void joystickConnected() { releaseAll(); commit(); };
  void joystickDisconnected() { releaseAll(); commit(); };

  void btnRedPress() { _report.button1 = 1; changed = true; }
  void btnRedRelease() { _report.button1 = 0; changed = true; }
  void btnBlackPress()  { _report.button2 = 1; changed = true; }
  void btnBlackRelease() { _report.button2 = 0; changed = true; }
  void axisXchange(int value) { _report.xAxis = value; changed = true; };
  void axisYchange(int value) { _report.yAxis = value; changed = true; };

  void commit() { if(changed) { SendReport(&_report, sizeof(_report)); changed = false; } }

protected: 
  inline void releaseAll(void) { memset(&_report, 0x00, sizeof(_report)); };

  HID_CoCoJoystickReport_Data_t _report;

  virtual inline void SendReport(void* data, int length) { 
      #ifdef DEBUG_REPORT
      Serial.print("REPORT ");
      Serial.print(DEFAULT_HID_REPORTID);
      Serial.print(" [");
      Serial.print(length, HEX);
      Serial.print("]: ");
      
      printHex((uint8_t *)data, length, true);
      #endif

      HID().SendReport(DEFAULT_HID_REPORTID, data, length);
  };
};

#endif
