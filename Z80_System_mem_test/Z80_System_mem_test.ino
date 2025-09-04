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

const uint16 MEM_MAX = 0x0080;
const uint16 MEM_START = 0x0000;
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
  lcdt.print(0,0, "System Starting.");

  Serial.begin(38400);
  while (! Serial) {}
  Serial.println("***********************");
  Serial.println("    system starting.    ");

  // nop test
  //m100bus.mem_disable();
  m100bus.clock_start(5, 4000);
  m100bus.address_bus16_mode(INPUT);
  m100bus.data_bus_mode(INPUT);

  lcdt.print(0,0, "Lowering BUSREQ");
  m100bus.BUSREQ(LOW);
  for(uint8 t = 0; t < 8; ++t) {
    m100bus.clock_wait_rising_edge();
    if ( !m100bus.BUSACK() ) break;
  }
  lcdt.print(1,0, "BUSACK.");  
  if (!m100bus.DMA_mode() ) {
    lcdt.print(0,0,"DMA mode failed.");
    while (true) ;
  } else {
    lcdt.print(0,0,"DMA mode.");
  
    for(uint16 ix = 0; ix < MEM_MAX - MEM_START; ++ix) {
      addr = MEM_START + ix;
      data = mem[ix];
      snprintf(buf, 32, "Write %02X to %04X", data, addr);
      lcdt.print(1,0,buf);
      m100bus.mem_write(addr, data);
      //delay(500);
    }

    lcdt.print(1,0,"Verifying.");
    uint16 errcount = 0;
    for(uint16 ix = 0; ix < MEM_MAX - MEM_START; ++ix) {
      addr = MEM_START + ix;
      data = m100bus.mem_read(addr);
      snprintf(buf, 32, "Read %02X from %04X", data, addr);
      lcdt.print(1,0,buf);
      if (data != mem[ix]) {
        errcount++;
        lcdt.print(2,0,"error!!");
        delay(1000);
      } else {
        lcdt.print(2,0,"Ok!!");
      }
    }
    if (errcount == 0) {
      lcdt.print(0,0,"Check passed.");
      delay(500);
    } else {
      snprintf(buf, 32, "Error: %d", errcount);
      lcdt.print(0,0,buf);
      delay(1000);//while (true);
    }
    m100bus.Z80_mode();
  }
  
  lcdt.print(0,0,"CPU Reset.");
  m100bus.Z80_RESET(LOW);
  m100bus.clock_wait_rising_edge(5);
  m100bus.Z80_RESET(HIGH);
  lcdt.clear();
}

void loop() {
  m100bus.clock_wait_rising_edge();
  if ( !m100bus.MREQ() && !m100bus.RD() ) {
    addr = m100bus.address_bus16_get();
    flag = m100bus.M1();
    m100bus.clock_wait_rising_edge();
    data = m100bus.data_bus_get();
    if (flag) {
      snprintf(buf, 32, "RD %.2s %04X  %02X", "M1", addr, data);
    } else {
      snprintf(buf, 32, "RD %.2s %04X  %02X", "  ", addr, data);
    }
    lcdt.print(0,0,buf);
  } else if ( !m100bus.MREQ() && !m100bus.WR() ) {
    addr = m100bus.address_bus16_get();
    m100bus.clock_wait_rising_edge();
    data = m100bus.data_bus_get();
    snprintf(buf, 32, "WR    %04X  %02X", addr, data);
    lcdt.print(0,0,buf);
  } 

}
