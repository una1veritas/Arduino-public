/* by sin, Aug, 2025 */
#include <LiquidCrystal.h>

#include "Z80Bus.h"
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
  SRAMEN   = 38, // E2 (positive neable)
};

Z80Bus z80bus(
  _INT, _NMI, _HALT, _MREQ, _IORQ, 
  _RD, _WR, _BUSACK, _WAIT, _BUSREQ, _RESET, _M1, _RFSH,
  SRAMEN);

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
uint8 dma_buff[256];

uint16 ram_check(uint32 start_addr = 0x0000, uint32 end_addr = 0x20000, uint32 skip = 0x0100) {
  uint8 buf[256];
  uint16 errcount = 0;
  randomSeed(analogRead(0));
  for(uint32 addr = start_addr; addr < end_addr; addr += skip) {
    uint8 t = random(256);
    Serial.print(addr, HEX);
    Serial.print(" -- ");
    Serial.print(addr + 0x100, HEX);
    Serial.print(", ");
    for(uint32 ix = 0; ix < 0x100; ++ix) {
      buf[ix] = z80bus.ram_read(addr + ix);
      buf[ix] ^= t;
      z80bus.ram_write(addr+ix, buf[ix]);
    }
    for(uint32 ix = 0; ix < 0x100; ++ix) {
      if (buf[ix] != z80bus.ram_read(addr+ix)) {
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
  Serial.println();
  return errcount;
}

void dump(uint8 mem_buff[], const uint16 size = 0x0100, const uint16 offset_addr = 0x0000) {
  char buf[16];
  uint8 data;
  for(uint32 ix = 0; ix < size; ++ix) {
    if ( (ix & 0x0f) == 0 ) {
      snprintf(buf, 16, "%04X : ", offset_addr+ix);
      Serial.print(buf);
    }
    snprintf(buf, 16, "%02x ", mem_buff[ix]);
    Serial.print(buf);
    if ( (ix & 0x0f) == 0x0f ) {
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
  //z80bus.mem_disable();
  z80bus.clock_start(4, 200);
  Serial.println("Reset Z80.");
  z80bus.cpu_reset();

  if (!z80bus.DMA_mode() ) {
    lcdt.print(0,0,"DMA mode failed.");
    while (true) ;
  } else {
    Serial.println("Entered DMA mode.");
    lcdt.print(0,0,"DMA mode.");

    Serial.println("Memory check...");
    ram_check(0x0000, 0x1000);

    uint8 res = z80bus.DMA_progmem_load(mem, MEM_MAX, 0x0000);
    if (res != 0) {
      Serial.println("Something going wrong w/ sram read & write!");
    }

    z80bus.DMA_address(0);
    z80bus.DMA_read(dma_buff);
    dump(dma_buff, 0x100, 0);
    z80bus.DMA_address(0x100);
    z80bus.DMA_read(dma_buff);
    dump(dma_buff, 0x100, 0x100);

    Serial.println("Exit to Z80 mode.");
    z80bus.Z80_mode();
  }
  Serial.println("_RESET goes HIGH.");
  z80bus.RESET(HIGH);
  lcdt.clear();
  lcdt.print(0,0,"Z80 starts.");
}

void loop() {
  z80bus.clock_wait_rising_edge();
  snprintf(buf, 21, "%-4s %-2s %-2s", 
  (z80bus.MREQ() ? (z80bus.IORQ() ? "" : "IORQ") : "MREQ"),
  (z80bus.M1() ? "" : "M1"),
  (z80bus.RD() ? (z80bus.WR() ? "" : "WR") : "RD")
  );
 // lcdt.print(0,0, buf);
  snprintf(buf, 21, "%-3s %-3s %-3s %-3s", 
  (z80bus.WAIT() ? "" : "WAT"),
  (z80bus.BUSACK() ? "" : "BAK"),
  (z80bus.HALT() ? "" : "HLT"),
  (z80bus.RFSH() ? "" : "RFH") );
  //lcdt.print(2,0, buf);

  if ( !z80bus.MREQ() ) {
    if ( ! z80bus.RD() ) {
      addr = z80bus.address_bus16_get();
      z80bus.clock_wait_rising_edge();
      data = z80bus.data_bus_get();
    } else if ( ! z80bus.WR() ) {
      addr = z80bus.address_bus16_get();
      z80bus.clock_wait_rising_edge();
      data = z80bus.data_bus_get();
    }
    z80bus.WAIT(LOW);
    snprintf(buf, 32, "%04X %02X", addr, data);
    lcdt.print(1,0,buf);
    z80bus.WAIT(HIGH);
  } else if ( !z80bus.IORQ() ) {
    if ( ! z80bus.RD() ) {
      // in operation
      addr = z80bus.address_bus16_get();
      z80bus.data_bus_mode_output();
      data = z80bus.z80io(addr, data, INPUT);
      z80bus.data_bus_set(data);
      z80bus.clock_wait_rising_edge(1);
      z80bus.clock_wait_rising_edge(1);
      z80bus.data_bus_mode_input();
    } else if ( ! z80bus.WR() ) {
      // out operation
      z80bus.data_bus_mode_input();
      addr = z80bus.address_bus16_get();
      z80bus.clock_wait_rising_edge(2);
      data = z80bus.data_bus_get();
      z80bus.z80io(addr, data, OUTPUT);
    } 
    z80bus.WAIT(LOW);
    snprintf(buf, 32, "%04X %02X", addr, data);
    lcdt.print(1,0,buf);
    z80bus.WAIT(HIGH);
  }
  if ( z80bus.DMA_requested() ) {
    Serial.println("Why? requested!");
    z80bus.DMA_exec(dma_buff);
    //dump(dma_buff, 0x100);
    z80bus.Z80_mode();
  } 
  if ( ! z80bus.HALT() ) {
    Serial.println("Halted.");
    if ( z80bus.DMA_mode() ) {
      Serial.println("Entered DMA mode.");
      lcdt.print(0,0,"DMA mode.");
      Serial.println("Memory dump...");
      z80bus.DMA_address(0);
      z80bus.DMA_read(dma_buff);
      dump(dma_buff, 0x100, 0);
      z80bus.DMA_address(0x100);
      z80bus.DMA_read(dma_buff);
      dump(dma_buff, 0x100, 0x100);

      Serial.println("Exit to Z80 mode.");
      z80bus.Z80_mode();
    }
    z80bus.clock_stop();
    while (true);
  }
}
