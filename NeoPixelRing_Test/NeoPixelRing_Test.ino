#include <Wire.h>
#include <RTC.h>

#include <Adafruit_NeoPixel.h>

RTC ds1307;
unsigned long clock, cal;
long prevmillis;
int otto;

const int PIN_DOUT = 6;

Adafruit_NeoPixel ring = Adafruit_NeoPixel(60, PIN_DOUT, NEO_GRB+NEO_KHZ800);
long swatch;

const int secwave[8][4] = {
  {26, 250, 26, 0},
  {13, 239, 46, 0},
  {7, 214, 72, 0},
  {3, 179, 106, 0},
  {1, 142, 142, 1},
  {0, 102, 183, 3},
  {0, 69, 217, 7},
  {0, 44, 241, 14},
};

const int ringperiod = 60;

void setup() {
  Serial.begin(19200);
  Wire.begin();
  ds1307.begin();
  ds1307.update();
  clock = ds1307.time;
  cal = ds1307.date;
  otto = millis()>>3;
  
  ring.begin();
  ring.setBrightness(32);
  ring.show();
  
}

void loop() {
  int sec;
  uint8_t val;
  
  if ( ((millis()/125)&0x07) != ((prevmillis/125)&0x07)  ) {
    prevmillis = millis();
    ds1307.updateTime();
    otto++;
    
    if ( clock != ds1307.time ) {
      clock = ds1307.time;
      ds1307.updateCalendar();
      cal = ds1307.date;
      otto = 0;
    }
    
    //const int shift = 5;
    sec = (clock & 0x0f) + (clock >> 4 & 0x0f)*10;
    //Serial.println(sec);
    for(int i = 0; i < ringperiod; i++) {      
      val = 0;
      if ( (0 <= (i+1+60-sec)%60) && ((i+1+60-sec)%60 < 4) ) {
        val = secwave[otto][(i+1+60-sec)%60];
      }
      ring.setPixelColor(i, ring.Color(val,0,0));
    }
    ring.show();
    
  }
}

