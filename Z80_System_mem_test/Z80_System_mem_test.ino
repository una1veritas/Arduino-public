/* by sin, Aug, 2025 */
#include <LiquidCrystal.h>

#include "Mega100Bus.h"
#include "progmem_rom.h"

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

struct Terminal {
  const LiquidCrystal & lcd;
  char line_buf[4][24];
  int row, col;

  Terminal(const LiquidCrystal & _lcd) : lcd(_lcd) {
    lcd.begin(20,4);
    lcd.clear();
    col = 0;
    row = 0;
  }

  void print(uint8 r, uint8 c, const char * str) {
    /*
    for(int i = 0; (c%20)+i < 20; ++i) {
      line_buf[r%4][(c%20)+i] = str[i];
    }
    */
    row = r % 4;
    snprintf(line_buf[row], 21, "%-20s", str);
    //line_buf[row][20] = '\0';
    lcd.setCursor(0,row);
    lcd.print(line_buf[row]);
  }

  void clear() {
    lcd.clear();
    row = 0;
    col = 0;
  }
} lcdt(lcd);

uint16 addr;
uint8 data;
uint8 flag;
char buf[32];

uint16 ram_check(uint32 start_addr = 0x0000, uint32 end_addr = 0x20000) {
  uint8 buf[256];
  uint32 skip = 0x0fe0;
  uint16 errcount = 0;
  randomSeed(analogRead(0));
  for(uint32 addr = start_addr; addr < end_addr; addr += skip) {
    uint8 t = random(256);
    Serial.print(addr, HEX);
    Serial.print(" -- ");
    Serial.println(addr + 0x100, HEX);
    for(uint32 ix = 0; ix < 0x100; ++ix) {
      buf[ix] = m100bus.ram_read(addr + ix);
      buf[ix] ^= t;
      m100bus.ram_write(addr+ix, buf[ix]);
    }
    for(uint32 ix = 0; ix < 0x100; ++ix) {
      if (buf[ix] != m100bus.ram_read(addr+ix)) {
        Serial.println(addr+ix, HEX);
        errcount++;
      }
    }
    if ( errcount > 0 ) {
      Serial.print(" errors = ");
      Serial.println(errcount, DEC);
    } else {
      // Serial.println(" no errors.");
    }
  }
  return errcount;
}

void ram_dump(uint32 baseaddr = 0x0000, const uint32 & bytes = 32) {
  char buf[16];
  uint8 data;
  baseaddr &= 0x1fff0;
  for(uint32 offset = 0; offset < bytes; ++offset) {
    if ( (offset & 0x0f) == 0 ) {
      snprintf(buf, 16, "%04X : ", baseaddr + offset);
      Serial.print(buf);
    }
    data = m100bus.ram_read(baseaddr + offset);
    snprintf(buf, 16, "%02x ", data);
    Serial.print(buf);
    if ( (offset & 0x0f) == 0x0f ) {
      Serial.println();
    }
    
  }
}

void setup() {
  // put your setup code here, to run once:
  lcdt.print(0,0, "System Starting.");

  Serial.begin(38400);
  while (! Serial) {}
  Serial.println("***********************");
  Serial.println("  Z80 system starting. ");
  Serial.println("***********************");

  // nop test
  //m100bus.mem_disable();
  m100bus.clock_start(5, 400);
  Serial.println("Reset Z80.");
  lcdt.print(0,0,"CPU Reset.");
  m100bus.cpu_reset();

  if (!m100bus.DMA_mode() ) {
    lcdt.print(0,0,"DMA mode failed.");
    while (true) ;
  } else {
    Serial.println("Entered DMA mode.");
    lcdt.print(0,0,"DMA mode.");

    Serial.println("Memory check...");
    ram_check(0x0000, 0x10000);

    uint8 res = m100bus.DMA_progmem_load(mem, MEM_MAX, 0x0000);
    if (res != 0) {
      Serial.println("Something going wrong w/ sram read & write!");
    }
    ram_dump(MEM_START,MEM_MAX);

    Serial.println("Exit to Z80 mode.");
    m100bus.Z80_mode();
  }
  Serial.println("_RESET goes HIGH.");
  m100bus.RESET(HIGH);
  lcdt.clear();
  lcdt.print(0,0,"Z80 starts.");
}

void loop() {
  m100bus.clock_wait_rising_edge();
  snprintf(buf, 21, "%-4s %-2s %-2s", 
  (m100bus.MREQ() ? (m100bus.IORQ() ? "" : "IORQ") : "MREQ"), 
  (m100bus.M1() ? "" : "M1"), 
  (m100bus.RD() ? (m100bus.WR() ? "" : "WR") : "RD") 
  );
  lcdt.print(0,0, buf);
  snprintf(buf, 21, "%-3s %-3s %-3s %-3s", 
  (m100bus.WAIT() ? "" : "WAT"), 
  (m100bus.BUSACK() ? "" : "BAK"), 
  (m100bus.HALT() ? "" : "HLT"),
  (m100bus.RFSH() ? "" : "RFH") );
  lcdt.print(2,0, buf);

  if ( !m100bus.MREQ() ) {
    if ( ! m100bus.RD() ) {
      addr = m100bus.address_bus16_get();
      m100bus.clock_wait_rising_edge();
      data = m100bus.data_bus_get();
    } else if ( ! m100bus.WR() ) {
      addr = m100bus.address_bus16_get();
      m100bus.clock_wait_rising_edge();
      data = m100bus.data_bus_get();
    }
    snprintf(buf, 32, "%04X %02X", addr, data);
    lcdt.print(1,0,buf);
  } else if ( !m100bus.IORQ() and m100bus.MREQ() ) {
    if ( ! m100bus.RD() ) {
      // in operation
      addr = m100bus.address_bus16_get();
      data = m100bus.ioport(addr, 0, INPUT);
      m100bus.data_bus_mode_output();
      m100bus.data_bus_set(data);
      m100bus.clock_wait_rising_edge(2);
      m100bus.data_bus_mode_input();
    } else if ( ! m100bus.WR() ) {
      // out operation
      m100bus.data_bus_mode_input();
      addr = m100bus.address_bus16_get();
      m100bus.clock_wait_rising_edge(2);
      data = m100bus.data_bus_get();
      m100bus.ioport(addr, data, OUTPUT);
    } 
    snprintf(buf, 32, "%04X %02X", addr, data);
    lcdt.print(1,0,buf);
  }
}
