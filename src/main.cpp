#define MAKE_TOUCH 1
#define REVERSE 0

#include <Adafruit_NeoPixel.h>
#include "Adafruit_MPR121.h"
#include "MultiTouch.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

const uint32_t SERIAL_BAUD = 115200;
const uint8_t TOUCH_PAD_NUM = 12;
const uint8_t NEO_PIXEL_PIN = 9;
const uint8_t NUM_PIXELS = 73;
const uint32_t TOUCH_SCREEN_MIN_X = 1000;
const uint32_t TOUCH_SCREEN_MAX_X = 9000;
const uint32_t TOUCH_SCREEN_Y = REVERSE ? 2000 : 8000;

Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_MPR121 cap = Adafruit_MPR121();

//const uint32_t COLOR_TOUCH = pixels.Color(85, 125, 255);  // AQUA
//const uint32_t COLOR_RELEASE = pixels.Color(153, 0, 255); // VIOLET
const uint32_t COLOR_TOUCH = pixels.Color(17, 37, 50);  // AQUA
const uint32_t COLOR_RELEASE = pixels.Color(12, 0, 12); // VIOLET

void bright(uint16_t touched);
uint32_t getColor(uint16_t state);
float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);

enum TOUCH_STATE{
  UP,
  DOWN,
  MOVING,
};

struct Touch_ {
  int8_t identifier; //-1: not used this identifier, 0~3:contact identifier
  uint8_t side_start;
  uint8_t side_end;
  float center;
  uint16_t bit_pattern;
  uint16_t bit_pattern_prev;
  TOUCH_STATE state; //0:touch down 1:moving 2:touch up
  Touch_(){
    identifier = -1;
    side_start = 0;
    side_end = 0;
    center = 0;
    bit_pattern = 0;
    bit_pattern_prev = 0;
    state = TOUCH_STATE::UP;
  }
};
typedef struct Touch_ Touch;

const uint8_t TOUCH_MAX = TOUCH_PAD_NUM / 2;
Touch prevTouch[TOUCH_MAX];
Touch curTouch[TOUCH_MAX];
uint8_t prev_touch_count = 0;
uint8_t cur_touch_count = 0;
uint8_t is_touch_processing = 0;
uint8_t using_identifier[TOUCH_MAX];
void makeTouch();

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
  cap.setThresholds(4, 6);
  cap.writeRegister(MPR121_DEBOUNCE, 0x25);
  cap.writeRegister(MPR121_CONFIG1, 0x30);
  cap.writeRegister(MPR121_CONFIG2, 0x20);
  Serial.println("MPR121 found!");
  pixels.begin();
  multiTouch.begin();
}

void loop()
{
  uint16_t current_touched = cap.touched();
  cur_touch_count = 0;
  for(uint8_t i = 0; i < TOUCH_PAD_NUM; i++){
    bool state = bool(current_touched & _BV(i));
    if(state && !is_touch_processing){
      is_touch_processing = 1;
      curTouch[cur_touch_count].side_start = i;
      curTouch[cur_touch_count].side_end = i;
    } else if(state && is_touch_processing){
      curTouch[cur_touch_count].side_end = i;
    } else if( (!state && is_touch_processing) || (state && i == (TOUCH_PAD_NUM-1)) ){
      is_touch_processing = 0;
      curTouch[cur_touch_count].center = float(curTouch[cur_touch_count].side_start + curTouch[cur_touch_count].side_end)/2;
      uint8_t cur_bit_num = curTouch[cur_touch_count].side_end - curTouch[cur_touch_count].side_start + 1;
      curTouch[cur_touch_count].bit_pattern = ( ( 1 << cur_bit_num) -1 ) << curTouch[cur_touch_count].side_start;
      uint8_t prev_bit_num = curTouch[cur_touch_count].side_end - curTouch[cur_touch_count].side_start + 3;
      curTouch[cur_touch_count].bit_pattern_prev = ( (1 << prev_bit_num) - 1)  << max((curTouch[cur_touch_count].side_start - 1), 0);
      cur_touch_count += 1;
    }
    if(state && i == (TOUCH_PAD_NUM-1)){
      curTouch[cur_touch_count].side_end = i;
      is_touch_processing = 0;
      curTouch[cur_touch_count].center = float(curTouch[cur_touch_count].side_start + curTouch[cur_touch_count].side_end)/2;
      uint8_t cur_bit_num = curTouch[cur_touch_count].side_end - curTouch[cur_touch_count].side_start + 1;
      curTouch[cur_touch_count].bit_pattern = ( ( 1 << cur_bit_num) -1 ) << curTouch[cur_touch_count].side_start;
      uint8_t prev_bit_num = curTouch[cur_touch_count].side_end - curTouch[cur_touch_count].side_start + 3;
      curTouch[cur_touch_count].bit_pattern_prev = ( (1 << prev_bit_num) - 1)  << max((curTouch[cur_touch_count].side_start - 1), 0);
      cur_touch_count += 1;
    }
  }

  uint8_t start_index = 0; //すでに移動したと判断したTouchは次のループで別のタッチに移動したと判断しないようにするため
  for(uint8_t i = 0; i < prev_touch_count; i++){
    bool touch_up = true;
    for(uint8_t j = start_index; j < cur_touch_count; j++){
      if(curTouch[j].bit_pattern & prevTouch[i].bit_pattern_prev){
        curTouch[j].identifier = prevTouch[i].identifier;
        curTouch[j].state = TOUCH_STATE::MOVING;
        start_index += 1;
        touch_up = false;
        break;
      }
    }
    if(touch_up){
      prevTouch[i].state = TOUCH_STATE::UP; //1個も近くにTouchが存在しなかったら、それは離されている
    }
  }
  //新しいTouchを探す
  for(uint8_t i = 0; i < cur_touch_count; i++){
    if(curTouch[i].identifier == -1){
      for(uint8_t identifier = 0; identifier < TOUCH_MAX; identifier++){ //使用されていないidentifierを探す
        if(using_identifier[identifier] == false){
          curTouch[i].identifier = identifier;
          curTouch[i].state = TOUCH_STATE::DOWN;
          using_identifier[identifier] = true;
          break;
        }
      }
    }
  }

  //ここで実際にタッチする
  if(MAKE_TOUCH)
    makeTouch();
  
  //タッチ状況をシリアル出力
  for(uint8_t i = 0; i < TOUCH_PAD_NUM; i++){
    bool state = bool(current_touched & _BV(i));
    Serial.print(state);
  }
  Serial.print("\r\n");

  //curTouchの中身をprevTouchにコピー
  memcpy(prevTouch, curTouch, sizeof(prevTouch));

  //curTouchの中身を初期化
  for(uint8_t i = 0; i < TOUCH_MAX; i++){
    curTouch[i].identifier = -1;
    curTouch[i].side_start = 0;
    curTouch[i].side_end = 0;
    curTouch[i].center = 0;
    curTouch[i].bit_pattern = 0;
    curTouch[i].bit_pattern_prev = 0;
    curTouch[i].state = TOUCH_STATE::UP;
  }
  prev_touch_count = cur_touch_count;

  //LEDテープを光らせる
  bright(current_touched);
  delay(5);
}

void makeTouch(){
  for(uint8_t i = 0; i < cur_touch_count; i++){
    float x = mapFloat(curTouch[i].center, 0., 11., TOUCH_SCREEN_MIN_X, TOUCH_SCREEN_MAX_X);
    float y = TOUCH_SCREEN_Y;
    if(REVERSE){
      float tmp = x;
      x = y;
      y = tmp;
    }
    if(curTouch[i].state == TOUCH_STATE::DOWN){
      //タッチされた瞬間の処理
      multiTouch.moveFingerTo(curTouch[i].identifier, x, y);
    } else if (curTouch[i].state == TOUCH_STATE::MOVING){
      //長押ししている間の処理
      multiTouch.moveFingerTo(curTouch[i].identifier, x, y);
    }
  }
  for(uint8_t i = 0; i < prev_touch_count; i++){
    if(prevTouch[i].state == TOUCH_STATE::UP){
      //離した瞬間の処理
      multiTouch.releaseFinger(prevTouch[i].identifier);
      using_identifier[prevTouch[i].identifier] = false; //identifierの解放
    }
  }
}

void bright(uint16_t touched){
  for (uint8_t touch_pos = 0; touch_pos < TOUCH_PAD_NUM; touch_pos++){
    for(uint8_t local_led_pos = 0; local_led_pos < 6; local_led_pos++){
      uint32_t color = getColor(touched & _BV(touch_pos));
      uint16_t global_led_pos = 1 + touch_pos * 6 + local_led_pos;
      if(global_led_pos == 1)
        pixels.setPixelColor(0, color);
      pixels.setPixelColor(1 + touch_pos * 6 + local_led_pos, color);
    }
  }
  pixels.show();
}

uint32_t getColor(uint16_t state){
  return state ? COLOR_TOUCH : COLOR_RELEASE;
}

float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow; 
}