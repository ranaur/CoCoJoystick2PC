#ifndef USBSingleCoCoJoystickEvent_h
#define USBSingleCoCoJoystickEvent_h

#include <Arduino.h>
#include "HID.h"
#include "CoCoJoystickEvent.h"
#define EPTYPE_DESCRIPTOR_SIZE      uint8_t

static const uint8_t _hidMultiReportDescriptorUSBSingleCoCoJoystick[] PROGMEM = {
  /* Gamepad with 2 buttons and 6 axis*/
  0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
  0x09, 0x04,             /* USAGE (Joystick) */
  0xa1, 0x01,             /* COLLECTION (Application) */
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

class USBSingleCoCoJoystickEvent : public CoCoJoystickEvent, public PluggableUSBModule {
  public:
  
  USBSingleCoCoJoystickEvent() : PluggableUSBModule(1, 1, epType), protocol(HID_REPORT_PROTOCOL), idle(1) {
    epType[0] = EP_TYPE_INTERRUPT_IN;
    PluggableUSB().plug(this);
  }

  private:
  int _buttonRed;
  int _buttonBlack;
  inline void releaseAll(void) { memset(&_report, 0x00, sizeof(_report)); };
  
  void setButtonNumbers(int buttonRed, int buttonBlack ) {
    _buttonRed = buttonRed;
    _buttonBlack = buttonBlack;
  }
  
  void joystickConnected() { releaseAll(); commit(); };
  void joystickDisconnected() { releaseAll(); commit(); };

  void btnRedPress() { _report.button1 = 1; }
  void btnRedRelease() { _report.button1 = 0; }
  void btnBlackPress()  { _report.button2 = 1; }
  void btnBlackRelease() { _report.button2 = 0; }
  void axisXchange(int value) { _report.xAxis = value; };
  void axisYchange(int value) { _report.yAxis = value; };

  void commit() { SendReport(&_report, sizeof(_report)); }

  private:
  HID_CoCoJoystickReport_Data_t _report;

  protected: /* for PluggableUSBModule */
    EPTYPE_DESCRIPTOR_SIZE epType[1];
    uint8_t protocol;
    uint8_t idle;

    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    
    virtual void SendReport(void* data, int length);
};
#endif
