#include <Wire.h>
#include <SPI.h>
#include <TimerOne.h>

#include <RTC.h>
#include <BMP085.h>

RTC rtc(RTC::CHIP_ST_M41T62);
BMP085 bmp;

void setup(void)
{
  Wire.begin();
  rtc.begin();
  rtc.start();
  bmp.begin();
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerCounter); // blinkLED to run every 0.15 seconds
  Serial.begin(9600);
}

volatile unsigned long timerCount = 0; // use volatile for shared variables

void timerCounter(void)
{
  timerCount++;
}

unsigned long getTimerCount() {
  unsigned long t;
  noInterrupts();
  t = timerCount;
  interrupts();
  return t;
}  

unsigned long last_RTC_update = 0;
enum BMPstatus { IDLE, TEMP_REQ, PRESS_REQ, } BMP_status = IDLE;
unsigned long BMP_wait;
unsigned long last_BMP_update;

void loop(void)
{
  unsigned int count = getTimerCount();
  
  if ( abs(count - last_RTC_update) > 1000 ) {
    last_RTC_update = count;
    rtc.update();
    Serial.print("time ");
    Serial.println(rtc.time, HEX);
    bmpRequest();
  }
  if ( bmpTask() ) {
    Serial.print("pressure ");
    Serial.println(float((bmp.pressure+5)/10) / 10.0, 1);
  }
  
  delay(150);
}

void bmpRequest() { 
  if (BMP_status == IDLE ) {
    BMP_status = TEMP_REQ;
    last_BMP_update = 0;
  }
}

boolean bmpTask() {
  if ( BMP_status == IDLE ) return false;
  unsigned long count = getTimerCount();
  if ( BMP_status == TEMP_REQ ) {
    if ( last_BMP_update == 0 ) {
      bmp.requestTemperature();
      last_BMP_update = count;
    } else {
      if ( abs(count - last_BMP_update) > bmp.waitOnRequest ) {
        bmp.getTemperature();
        last_BMP_update = 0;
        BMP_status = PRESS_REQ;
      }
    }
  } else if ( BMP_status == PRESS_REQ ) {
    if ( last_BMP_update == 0 ) {
      bmp.requestPressure(1);
      last_BMP_update = count;
    } else {
      if ( abs(count - last_BMP_update ) > bmp.waitOnRequest ) {
        bmp.getPressure();
        last_BMP_update = 0;
        BMP_status = IDLE;
        return true;
      }
    }
  }
  return false;
}

