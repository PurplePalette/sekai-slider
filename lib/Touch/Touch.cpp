#include "Touch.h"

#if defined(_USING_HID)

#define CONTACT_COUNT_MAXIMUM 10
#define REPORTID_TOUCH        0x04

#define LSB(v) ((v >> 8) & 0xff)
#define MSB(v) (v & 0xff)

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  0x05, 0x0D,                    // USAGE_PAGE(Digitizers)
  0x09, 0x04,                    // USAGE     (Touch Screen)
  0xA1, 0x01,                    // COLLECTION(Application)
  0x85, REPORTID_TOUCH,          //   REPORT_ID (Touch)

  // define the maximum amount of fingers that the device supports
  0x09, 0x55,                    //   USAGE (Contact Count Maximum)
  0x25, CONTACT_COUNT_MAXIMUM,   //   LOGICAL_MAXIMUM (CONTACT_COUNT_MAXIMUM)
  0xB1, 0x02,                    //   FEATURE (Data,Var,Abs)

  // define the actual amount of fingers that are concurrently touching the screen
  0x09, 0x54,                    //   USAGE (Contact count)
  0x95, 0x01,                    //   REPORT_COUNT(1)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)

  // declare a finger collection
  0x09, 0x22,                    //   USAGE (Finger)
  0xA1, 0x02,                    //   COLLECTION (Logical)

  // declare an identifier for the finger
  0x09, 0x51,                    //     USAGE (Contact Identifier)
  0x75, 0x08,                    //     REPORT_SIZE (8)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)

  // declare Tip Switch and In Range
  0x09, 0x42,                    //     USAGE (Tip Switch)
  0x09, 0x32,                    //     USAGE (In Range)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x95, 0x02,                    //     REPORT_COUNT(2)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)

  // declare the remaining 6 bits of the first data byte as constant -> the driver will ignore them
  0x95, 0x06,                    //     REPORT_COUNT (6)
  0x81, 0x03,                    //     INPUT (Cnst,Ary,Abs)

  // define absolute X and Y coordinates of 16 bit each (percent values multiplied with 100)
  0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,                    //     Usage (X)
  0x09, 0x31,                    //     Usage (Y)
  0x16, 0x00, 0x00,              //     Logical Minimum (0)
  0x26, 0x10, 0x27,              //     Logical Maximum (10000)
  0x36, 0x00, 0x00,              //     Physical Minimum (0)
  0x46, 0x10, 0x27,              //     Physical Maximum (10000)
  0x66, 0x00, 0x00,              //     UNIT (None)
  0x75, 0x10,                    //     Report Size (16),
  0x95, 0x02,                    //     Report Count (2),
  0x81, 0x02,                    //     Input (Data,Var,Abs)
  0xC0,                          //   END_COLLECTION
  0xC0                           // END_COLLECTION

  // with this declaration a data packet must be sent as:
  // byte 1   -> "contact count"        (always == 1)
  // byte 2   -> "contact identifier"   (any value)
  // byte 3   -> "Tip Switch" state     (bit 0 = Tip Switch up/down, bit 1 = In Range)
  // byte 4,5 -> absolute X coordinate  (0...10000)
  // byte 6,7 -> absolute Y coordinate  (0...10000)
};


/*static const uint8_t _hidReportDescriptor[] PROGMEM = {
          0x05, 0x0D,                     // Usage Page (Digitizer)
        0x09, 0x04,                     // Usage (Touch Screen)
        0xa1, 0x01,                     // Collection (Application)
        0x09, 0x22,                     //   Usage (Finger)
        0xA1, 0x02,                     //   Collection (Logical)
        0x09, 0x42,                     //     Usage (Tip Switch)
        0x15, 0x00,                     //     Logical Minimum (0)
        0x25, 0x01,                     //     Logical Maximum (1)
        0x75, 0x01,                     //     Report Size (1)
        0x95, 0x01,                     //     Report Count (1)
        0x81, 0x02,                     //     Input (variable,absolute)
        0x09, 0x51,                     //     Usage (Contact Identifier)
        0x25, 0x7F,                     //     Logical Maximum (127)
        0x75, 0x07,                     //     Report Size (7)
        0x95, 0x01,                     //     Report Count (1)
        0x81, 0x02,                     //     Input (variable,absolute)
        0x09, 0x30,                     //     Usage (Pressure)
        0x26, 0xFF, 0x00,               //     Logical Maximum (255)
        0x75, 0x08,                     //     Report Size (8)
        0x95, 0x01,                     //     Report Count (1)
        0x81, 0x02,                     //     Input (variable,absolute)
        0x05, 0x01,                     //     Usage Page (Generic Desktop)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x26, 0xFF, 0x7F,               //     Logical Maximum (32767)
        0x65, 0x00,                     //     Unit (None)  <-- probably needs real units?
        0x75, 0x10,                     //     Report Size (16)
        0x95, 0x02,                     //     Report Count (2)
        0x81, 0x02,                     //     Input (variable,absolute)
        0xC0,                           //   End Collection
        0x05, 0x0D,                     //   Usage Page (Digitizer)
        0x27, 0xFF, 0xFF, 0, 0,         //   Logical Maximum (65535)
        0x75, 0x10,                     //   Report Size (16)
        0x95, 0x01,                     //   Report Count (1)
        0x09, 0x56,                     //   Usage (Scan Time)
        0x81, 0x02,                     //   Input (variable,absolute)
        0x05, 0x0D,                     //   Usage Page (Digitizers)
        0x09, 0x55,                     //   Usage (Contact Count Maximum)
        0x25, CONTACT_COUNT_MAXIMUM,       //   Logical Maximum (10)
        0x75, 0x08,                     //   Report Size (8)
        0x95, 0x01,                     //   Report Count (1)
        0xB1, 0x02,                     //   Feature (variable,absolute)
        0xC0                            // End Collection
};*/

typedef struct Finger {
  int contact;
  int16_t x;
  int16_t y;
} Finger;

Finger fingers[CONTACT_COUNT_MAXIMUM];


Touch_::Touch_() {
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);

  for (int i = 0; i < CONTACT_COUNT_MAXIMUM; i++) {
    fingers[i].contact = 0;
    fingers[i].x = 0;
    fingers[i].y = 0;
  }
}

void Touch_::begin(void) {
}

void Touch_::end(void) {
}


void Touch_::send(uint8_t identifier, uint8_t touch, int16_t x, int16_t y) {
  // calculate current contact count
  uint8_t contact = 0;
  
  for (int i = 0; i < CONTACT_COUNT_MAXIMUM; i++) {
    if (fingers[i].contact) {
      contact += 1;
    }
  }

  // create data array
  uint8_t data[] = {
    contact,           // contact count
    identifier,        // contact identifier
    touch,             // finger touches display
    MSB(x), LSB(x),    // x
    MSB(y), LSB(y)     // y
  };

  // send packet
  HID().SendReport(REPORTID_TOUCH, data, 7);
}


void Touch_::moveFingerTo(uint8_t identifier, int16_t x, int16_t y) {
  // change finger record
  fingers[identifier].contact = 1;
  fingers[identifier].x = x;
  fingers[identifier].y = y;

  // send update
  send(identifier, 1, x, y);
}

void Touch_::releaseFinger(uint8_t identifier) {
  // change finger record
  fingers[identifier].contact = 0;
  fingers[identifier].x = 0;
  fingers[identifier].y = 0;

  // send update
  send(identifier, 0, 0, 0);
}

Touch_ Touch;

#endif