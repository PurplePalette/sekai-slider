#ifndef Touch_h
#define Touch_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

class MultiTouch_ {
private:
  void send(uint8_t identifier, uint8_t touch, int16_t x, int16_t y);
public:
  MultiTouch_(void);
  void begin(void);
  void end(void);
  void moveFingerTo(uint8_t finger, int16_t x, int16_t y);
  void releaseFinger(uint8_t finger);
};
extern MultiTouch_ MultiTouch;

#endif
#endif