
#include <Adafruit_GFX.h>    // Core graphics library
#include "GPIOBus.h"
//#include <SPI.h>
//#include "MCP23Sxx.h"
#include "TFTLCD.h" // Hardware-specific library

#define LCD_CS 18 // Chip Select goes to Analog 3
#define LCD_CD 19 // Command/Data goes to Analog 2
#define LCD_WR 20 // LCD Write goes to Analog 1
#define LCD_RD 21 // LCD Read goes to Analog 0

#define LCD_RESET 0xff // Can alternately just connect to Arduino's reset pin


#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//MCP23Sxx ioext(10);
GPIOBus gpiobus;

TFTLCD tft(gpiobus, LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

void setup(void) {
  
  Serial.begin(57600);
  Serial.println(F("TFT LCD test"));
  Serial.println(F("Using IO Extender."));
  Serial.flush();
  
//  SPI.begin();
//  ioext.begin();
  tft.begin();
    
  switch(tft.driverID()) {
  case ID_932X:
    Serial.println(F("Found ILI932X LCD driver"));
    break;
  case ID_7575:
    Serial.println(F("Found HX8347G LCD driver"));
    break;
  default:
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(tft.readID(), HEX);
    Serial.println("Halt.");
    while (1) ;
    break;
  }

//while (1);
  Serial.println();
  Serial.println(F("Benchmark                Time (microseconds)"));
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);
}

void loop(void) {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    testText();
    delay(2000);
  }
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}


