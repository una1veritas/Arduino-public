#include <Wire.h>
#include "OV7670.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include "TFTLCD.h" // Hardware-specific library
#include "gpio.h"

TFTLCD tft;
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

static const GPIOPin VSYNC = GPIOD3; //  5: VSYNC, /WRST
static const GPIOPin HREF  = GPIOD4; //  6: HREF (NAND(WEN,HREF) --> /WE) 
static const GPIOPin WEN   = GPIOD5; //  7: WEN (NAND(WEN,HREF) --> /WE) 
                             //  8: XCK
static const GPIOPin RRST = GPIOB2; //  9: read-reset, low resets fifo read counter
static const GPIOPin OE   = GPIOB3; // 10: OE, output enable

//OCR2 (OC2B)
static const GPIOPin RCLK = GPIOD6; // 11: read out clock, 
//data are valid on riging edges

OV7670 cam(VSYNC, HREF, WEN, RCLK, RRST, OE);
  
long lastshot = 0;

void setup() {
  // put your setup code here, to run once:

  delay(500);
  Serial.begin(57600);
  Serial.println(F("Serial started."));

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  Wire.begin();
  Serial.println(F("Wire started."));
  if ( cam.begin() )
    Serial.println(F("Cam started."));
  else
    Serial.println(F("Cam failed."));
  cam.mirrorflip(0);
  cam.maxgain(1);
  cam.enhanceEdge(1);
  //cam.contrast(0x40);
  /*
  delay(10);
  Serial.print("GAIN = ");
  Serial.println(cam.readRegister(REG_GAIN), HEX);
  Serial.print("BLUE = ");
  Serial.println(cam.readRegister(REG_BLUE), HEX);
  Serial.print("RED = ");
  Serial.println(cam.readRegister(REG_RED), HEX);
  Serial.print("COM7 = ");
  Serial.println(cam.readRegister(REG_COM7), HEX);
  Serial.print("COM8 = ");
  Serial.println(cam.readRegister(REG_COM8), HEX);
  Serial.print("COM9 = ");
  Serial.println(cam.readRegister(REG_COM9), HEX);
  for(int i = 0x4f; i <= 0x58; i++) {
    Serial.print(" Reg "); Serial.print(i, HEX); Serial.print(" = ");
    Serial.println(cam.readRegister(i), HEX);
  }
  */
  
  cam.outputDisable();
  //cam.startRCLK(1);
  DDRA = 0x00;
}

void loop() {
  uint16_t color, red, blue, green, tmp;
  
  if ( millis() - lastshot > 1000 ) {
    cam.waitVSYNCFalling();
    cam.writeDisable();
    cam.waitVSYNCFalling();
    cam.writeEnable();
    cam.outputEnable();
    cam.RRST(LOW);
    cam.RCLK(LOW);
    __asm__ __volatile__("nop");
    cam.RCLK(HIGH);
    cam.RRST(HIGH);
    __asm__ __volatile__("nop");
    cam.RCLK(LOW);
    __asm__ __volatile__("nop");
    cam.RCLK(HIGH);
    __asm__ __volatile__("nop");
    //
    for(int row = 0; row <240; row++) {
      tft.drawPixel(0,row,0);
      for(int col = 0; col < 320 ; col++) {
        cam.RCLK(LOW);
        color = PINA;
        color <<= 8;
        cam.RCLK(HIGH);
        __asm__ __volatile__("nop");
        cam.RCLK(LOW);
        color |= PINA;
        cam.RCLK(HIGH);
        /*
        tmp = (color & 0x001f)<<6;
        tmp |= (color & 0x07c0)>>6;
        tmp |= (color & 0xf800);
        color = tmp;
        */
        // becasue LCD is originally in portrait mode.
        //tft.drawPixel(row, col, color);
        tft.drawPixel(color);
      }
    }
    cam.outputDisable();
    //
    
    lastshot = millis();
  }
}

