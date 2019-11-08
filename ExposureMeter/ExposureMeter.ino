#include <SoftwareSerial.h>
#include <Wire.h>
#include "CharacterLCD.h"
#include "ST7032i.h"

long swatch;
int values[256];
long sum = 0;
int index;
int ledbrt;
//SoftwareSerial softser(2,3);
ST7032i lcd; 
int lcd_light;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(19200);
  //softser.begin(9600);
  //softser.write(0x76);
  //ledbrt = 100;
  //softser.write(0x7a);
  //softser.write(ledbrt);
  Wire.begin();
  lcd.begin();
  lcd.setContrast(34);
  pinMode(14, INPUT);
  digitalWrite(14, LOW);
  lcd_light = 0;
  
  swatch = micros();
  for(int i = 0; i < 256; ++i) {
    values[i] = analogRead(3);
    sum += values[i];
  }
  index = 0;
}

// 30k
// 真っ暗 100
// 北向き窓の部屋薄カーテン越し 450 - 480
// ビルの日向の照り返し 573
// 北側の通路 525
// 南向きの部屋昼 493 - 503
// 薄曇りの日の南の空と風景 575
// 南のベランダの床 560 - 570 
// ひなたで太陽に向ける 604

void loop() {
  int avr;
  // put your main code here, to run repeatedly:
  if ( abs(micros() - swatch) > 500 ) {
    swatch = micros();
    sum -= values[index];
    values[index] = analogRead(3);
    sum += values[index];
    avr = sum>>8;
    index = (index + 1) & 0xff;
    if ( index == 0 ) {
      //Serial.println(avr);
      //softser.write(avr/1000 % 10);
      //softser.write(avr/100 % 10);
      //softser.write(avr/10 % 10);
      //softser.write(avr % 10);
      lcd.setCursor(0,0);
      lcd.print(avr);
      lcd.print("    ");
      lcd.setCursor(0,1);
      lcd.print(5*avr/1024.0, 2);
      lcd.print("    ");

      if (!lcd_light && avr < 657 ) {
        pinMode(14, OUTPUT);
        lcd_light = 1;
      } else if (lcd_light && avr > 678 ) {
        pinMode(14, INPUT);
        lcd_light = 0;
      }
    }
  }
}
