/* by sin, Aug, 2025 */
#include <LiquidCrystal.h>

#include "Mega100Bus.h"

//#define BUS_DEBUG

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

enum Mega_pin_assign {
  // z80
  CLK     = 13, // input on Z80
  _INT    = 3,  // in
  _NMI    = 4,  // in
  _HALT   = 5, // output on Z80
  _MREQ   = 39, // out
  _IORQ   = 6, // out 
  _RD     = 40, // out
  _WR     = 41, // out
  _BUSACK =  7,  // out
  _WAIT   =  8,  // in
  _BUSREQ =  9,  // in 
  _RESET  = 10,  // in
  _M1     = 11,  // out
  _RFSH   = 12,  // out

  // sram
  MEMEN   = 38, // E2 (positive neable)
};

Mega100Bus m100bus(
  //CLK, fixed
  _INT, _NMI, _HALT, _MREQ, _IORQ, 
  _RD, _WR, _BUSACK, _WAIT, _BUSREQ, _RESET, _M1, _RFSH,
  MEMEN);

const int LCD_RS = 14, LCD_EN = 15, LCD_D4 = 16, LCD_D5 = 17, LCD_D6 = 18, LCD_D7 = 19;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void LCD_show_bus(uint16 addr, uint8 data) {
  char buff[24];
  snprintf(buff, sizeof(buff), "%04X %02x", addr, data);
  lcd.print(buff);
}
uint16 addr;
uint8 data;
uint8 flag;
char strbuf[32];

const uint16 MEM_MAX = 0x0080;
const uint16 MEM_ADDR_MASK = MEM_MAX - 1;
uint8 mem[MEM_MAX] = {
  /* example 2
  0x31, 0x30, 0x00, 0x21, 0x13, 0x00,
  0xcd, 0x0a, 0x00, 
  0x76, 
  0x7e, 
  0xb7, 
  0xc8, 
  0xd3, 0x02, 
  0x23, 
  0xc3, 0x0a, 0x00,
  0x0d, 0x0a, 
  0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 
  0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0d, 0x0a, 0x00,
  */
  /* example 1 */
0x21, 0x38, 0x00, 0x31, 0x7e, 0x00, 0xaf, 0x77, 0x3c, 0xfe, 0x64, 0x28, 0x08, 0xcd, 0x12, 0x00, 
0x18, 0xf5, 0xd3, 0xff, 0xc9, 0x21, 0x23, 0x00, 0x7e, 0xf6, 0x00, 0x28, 0x05, 0xd3, 0x02, 0x23, 
0x18, 0xf6, 0x76, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x6d, 0x79, 0x20, 0x66, 0x72, 0x69, 
0x65, 0x6e, 0x64, 0x73, 0x21, 0x0d, 0x0a, 0x00, 0x00,
};
 

void setup() {
  // put your setup code here, to run once:
  lcd.begin(20, 4);
  lcd.clear();
  lcd.print("system starting...");

  Serial.begin(38400);
  while (! Serial) {}
  Serial.println("***********************");
  Serial.println("    system starting.    ");

  // nop test
  m100bus.clock_start(5, 8000);

  if (!m100bus.DMA_mode() ) {
    lcd.clear();
    lcd.write("set dma mode failed.");
    while (true) ;
  }
  randomSeed(analogRead(0));
  
  for(uint16 i = 0; i < 100; ++i) {
    uint16 base_addr = random(256)<<8;
    lcd.clear();
    lcd.setCursor(0,0);
    LCD_show_bus(base_addr, i);
    lcd.setCursor(0,1);
    lcd.print("writing");
    for(uint16 addr = 0; addr < 0x40; ++addr) {
      mem[addr] = random(256);
      m100bus.mem_write(base_addr+addr, mem[addr]);
    }
    delay(500);
    lcd.setCursor(0,1);
    lcd.print("reading & verifying");
    uint16 errcount = 0;
    for(uint16 addr = 0; addr < 0x40; ++addr) {
      uint8 data = m100bus.mem_read(base_addr+addr);
      if (data != mem[addr]) {
        errcount += 1;
      }
    }
    lcd.setCursor(0,2);
    lcd.print("    ");
    lcd.setCursor(0,2);
    if (errcount > 0) {
      lcd.print("OK. ");
    } else {
      lcd.print("Error occurred: ");
      lcd.setCursor(0,3);
      lcd.print(errcount);
      while (true);
    }
    delay(1000);
  }
  m100bus.Z80_mode();
  Serial.println("done.");
}

void loop() {

}
