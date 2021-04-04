#include <Adafruit_NeoPixel.h>
#include "Adafruit_MPR121.h"
#ifdef __AVR__
#include <avr/power.h>
#endif
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

const uint32_t SERIAL_BAUD = 115200;
const uint8_t NEO_PIXEL_PIN = 9;
const uint8_t NUM_PIXELS = 73;

Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_MPR121 cap = Adafruit_MPR121();

//const uint32_t COLOR_TOUCH = pixels.Color(85, 125, 255);  // AQUA
//const uint32_t COLOR_RELEASE = pixels.Color(153, 0, 255); // VIOLET
const uint32_t COLOR_TOUCH = pixels.Color(55, 95, 125);  // AQUA
const uint32_t COLOR_RELEASE = pixels.Color(100, 0, 100); // VIOLET

void bright(uint16_t touched);
uint32_t getColor(bool state);

void setup()
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A))
  {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  pixels.begin();
}

void loop()
{
  uint16_t current_touched = cap.touched();

  for(uint8_t i = 0; i < 12; i++)
    Serial.print(bool(current_touched & _BV(i)));
  Serial.print("\r\n");

  bright(current_touched);
  delay(5);
}

void bright(uint16_t touched){
  for (uint8_t touch_pos=0; touch_pos<12; touch_pos++){
    for(uint8_t local_led_pos=0; local_led_pos<6; local_led_pos++){
      uint32_t color = getColor(touched & _BV(touch_pos));
      uint16_t global_led_pos = 1 + touch_pos * 6 + local_led_pos;
      if(global_led_pos == 1)
        pixels.setPixelColor(0, color);
      pixels.setPixelColor(1 + touch_pos * 6 + local_led_pos, color);
    }
  }
  pixels.show();
}

uint32_t getColor(bool state){
  return state ? COLOR_TOUCH : COLOR_RELEASE;
}