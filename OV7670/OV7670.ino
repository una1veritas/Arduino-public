#include <Wire.h>
#include "gpio.h"
#include "OV7670.h"

static const byte VSYNC = 3; //  5: VSYNC, /WRST
static const byte HREF  = 4; //  6: HREF (NAND(WEN,HREF) --> /WE) 
static const byte WEN   = 5; //  7: WEN (NAND(WEN,HREF) --> /WE) 
                             //  8: XCK
static const byte RRST = 14; //  9: read-reset, low resets fifo read counter
static const byte OE   = 15; // 10: OE, output enable

//OCR2 (OC2B)
static const byte RCLK = 6; // 11: read out clock, 
//data are valid on riging edges

OV7670 cam(OE);

bool shot;

void setup() {
  // put your setup code here, to run once:

  delay(500);
  Serial.begin(9600);
  Serial.println(F("Serial started."));

  Wire.begin();
  Serial.println(F("Wire started."));
  if ( cam.begin() )
    Serial.println(F("Cam started."));
  else
    Serial.println(F("Cam failed."));
    
  cam.outputDisable();
    
  pinMode(WEN, OUTPUT); // WEN
  digitalWrite(WEN, LOW);
  pinMode(VSYNC, INPUT);  // VSYNC
  pinMode(RRST, OUTPUT);
  digitalWrite(RRST, HIGH);

 // RCLK 1MHz  by TC2 fast-PWM with TOP=OCRA  
  pinMode(RCLK, OUTPUT);  
  TCCR2A =  _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B =  _BV(WGM22) | _BV(CS20); //| _BV(CS21) | _BV(CS22);
  OCR2A = 31;
  OCR2B = 15;
  Serial.println("RCLK started.");

  
  delay(10);
  Serial.print("COM7 = ");
  Serial.flush();
  Serial.println(cam.readRegister(OV7670::OV7670_COM7), HEX);
  Serial.print("AECHH = ");
  Serial.println(cam.readRegister(OV7670::OV7670_AECHH), HEX);

  shot = true;
}

void loop() {
    if ( digitalRead(VSYNC) == LOW && shot) {
      while (digitalRead(VSYNC) == LOW) {}
      digitalWrite(WEN, HIGH);
      delayMicroseconds(100);
      digitalWrite(OE,LOW);
      //
      while (digitalRead(HREF) == LOW) {}
      digitalWrite(RRST,LOW);
      digitalWrite(RRST,HIGH);
      for(int cnt = 0; cnt < 120+2; cnt++) {
        while (digitalRead(HREF) == LOW) {}
        while (digitalRead(HREF) == HIGH) {}
      }
      digitalWrite(OE,HIGH);
      //
      while (digitalRead(VSYNC) == HIGH) {} 
      digitalWrite(WEN, LOW);
      shot = false;
    }
}

