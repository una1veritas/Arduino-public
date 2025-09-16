/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 4
 * LCD Enable pin to digital pin 5
 * LCD D4 pin to digital pin 0
 * LCD D5 pin to digital pin 1
 * LCD D6 pin to digital pin 2
 * LCD D7 pin to digital pin 3
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe 
 
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <Wire.h>
#include "IOExpander.h"
#include "MCP230xx.h"
#include "CharacterLCD.h"
#include "MCP230xxLCD.h"

// initialize the library with the numbers of the interface pins
// pin assignments for one of my LCD adapters
MCP230xxLCD lcd(0x07, 0,1,2, 4,5,6,7, 3); //6,5,4,3,2,1,0, 7);
//MCP230xxLCD lcd(0x00, 1, 0xff, 2, 3, 4, 5, 6, 7); // for adafruit i2c/spi lcd adapter

void setup() {
  Wire.begin();
  // set up the LCD's number of rows and columns: 
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.backlightOn();
  lcd.print("Hello, world!");
  for(int i = 0; i < 3; i++) {
    delay(750);
    lcd.noDisplay();
    delay(250);
    lcd.display();
  }
}
uint8_t c;
void loop() {
  c++;
  //lcd.command(c);
  delay(100);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/10);
}

