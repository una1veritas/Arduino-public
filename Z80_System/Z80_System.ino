/* by sin, Aug, 2025 */
#include <SPI.h>
//#include <LiquidCrystal.h>

#include "Z80Bus.h"

//#define BUS_DEBUG

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

/*
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
*/

const int SPI_CS = 53;//latchPin = 53; --- must be controlled by user
//const int SPI_CLK = 52; //clockPin = 52; --- controlled by SPI module.
//const int SPI_COPI = 51; //dataPin = 51; --- controlled by SPI module.

byte ascii7seg(byte ch) {
  const static uint8_t numeric7[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff };
  const static uint8_t alpha7[] = { 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xc2, 0x89, 0x79, 0xe1,
    0x09 /* 0x89*/, 0xc7, 0xbb /* 0xaa*/, 0xab /* 0xc8*/, 0xa3, 0x8c, 0x98, 0xaf /* 0xce */, 0x92 /*0x9b*/,
    0x87, 0xc1, 0xc1 /* 0xe3*/, 0x86 /* 0xd5 */, 0xb6, 0x91, 0x24 /* 0xb8*/,
  };  //a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z
  if ('0' <= ch and ch <= '9') {
    return numeric7[ch - '0'];
  }
  switch(ch){
    case '.':
    case ',':
      return 0x7f;
      break;
    case ' ':
      return 0xff;
      break;
    case 'i':
      return 0x7b;
      break;
    case 'o':
      return 0xa3;
      break;
    case 'u':
      return 0xe3;
      break;
  }
  ch = toupper(ch);
  if ('A' <= ch and ch <= 'Z') {
    return alpha7[ch - 'A'];
  } 
  return 0xff;
}


//uint16_t addr;
//uint8_t data;
char busmode = ' ';
long prev_millis;
char buf[32];
uint8_t dma_buff[256];

void dump(uint8_t mem_buff[], const uint16_t size = 0x0100, const uint16_t offset_addr = 0x0000) {
  //char buf[16];
  uint8_t data;
  for(uint32_t ix = 0; ix < size; ++ix) {
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
  //lcdt.print(0,0, "System Starting.");

  Serial.begin(115200);
  while (! Serial) {}
  Serial.println("***********************");
  Serial.println("  Z80 system starting. ");
  Serial.println("***********************");

  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);
  // SPI.setClockDivider(SPI_CLOCK_DIV4);  -- div4 seems to be the default clock divider value
  SPI.begin();

  // nop test
  //z80bus.mem_disable();
  z80bus.clock_set(2, 1000); 
  z80bus.clock_start();
  Serial.println("Reseting Z80...");
  z80bus.cpu_reset();

  if (!z80bus.DMA_mode() ) {
    //lcdt.print(0,0,"DMA mode failed.");
    while (true) ;
  } else {
    Serial.println("Entered DMA mode.");
    //lcdt.print(0,0,"DMA mode.");

    
    Serial.print("Memory check...");
    uint16_t errcount = z80bus.ram_check(0x0000, 0x1000);
    if ( errcount > 0 ) {
      Serial.println();
      Serial.print("Warning!!! Mem check encountered ");
      Serial.print(errcount);
      Serial.println(" errors.");
    } else {
      Serial.println(" Ok.");
    }

    // Load bootloader from 0x0000 by  DMA, arduino to ram 
    uint8_t res = z80bus.DMA_progmem_load(Z80Bus::boot_0000, 0x0000, 256);
    if (res != 0) {
      Serial.println("Something going wrong w/ sram read & write!");
    } else {
      Serial.println("Transfer completed.");
    }
    
    // test DMA, ram to arduino 
    /*
    z80bus.DMA_address(0);
    z80bus.DMA_read(dma_buff);
    dump(dma_buff, 0x100, 0);
    z80bus.DMA_address(0x100);
    z80bus.DMA_read(dma_buff);
    dump(dma_buff, 0x100, 0x100);
    */
    Serial.println("Exit to Z80 mode.");
    z80bus.MMC_mode();
  }
  Serial.println("_RESET goes HIGH.");
  z80bus.RESET(HIGH);
  //lcdt.clear();
  //lcdt.print(0,0,"Z80 starts.");
  prev_millis = millis();
}

void loop() {
  uint8_t data;
  uint16_t addr;
  uint32_t val;
  z80bus.clock_wait_rising_edge();
   if ( !z80bus.MREQ() ) {
    if ( ! z80bus.RD() ) {
      /*
      addr = z80bus.address_bus16_get();
      z80bus.clock_wait_rising_edge();
      data = z80bus.data_bus_get();
      */
      val = z80bus.mem_rw();
      busmode = 'r';      
    } else if ( ! z80bus.WR() ) {
      /*
      addr = z80bus.address_bus16_get();
      z80bus.clock_wait_rising_edge();
      data = z80bus.data_bus_get();
      */
      val = z80bus.mem_rw();
      busmode = 'u';
    } // else RFSH
  } else if ( !z80bus.IORQ() ) {
    if ( ! z80bus.RD() ) {
      // in operation
      /*
      addr = z80bus.address_bus16_get();
      z80bus.data_bus_mode_output();
      */
      val = z80bus.io_rw();
      /*
      z80bus.data_bus_set(data);
      z80bus.clock_wait_rising_edge(1);
      z80bus.clock_wait_rising_edge(1);
      z80bus.data_bus_mode_input();
      */
      busmode = 'i';
    } else if ( ! z80bus.WR() ) {
      // out operation
      /*
      z80bus.data_bus_mode_input();
      addr = z80bus.address_bus16_get();
      z80bus.clock_wait_rising_edge(2);
      data =   z80bus.data_bus_get();
      */
      val = z80bus.io_rw();
      busmode = 'o';
    } 
  }

  if (z80bus.clock_mode_current() <= 4) {
	  addr = val>>16;
	  data = val & 0xff;
	  if ( busmode == 'o' or busmode == 'i') {
		addr &= 0xff;
		snprintf(buf, 9, "  %02X%c %02X", addr, busmode, data );
	  } else {
		snprintf(buf, 9, "%04X%c %02X", addr, busmode, data );
	  }
	  digitalWrite(SPI_CS, LOW);
	  for (int i = 8; i > 0; ) {
		  SPI.transfer(ascii7seg(buf[--i]));
	  }
	  digitalWrite(SPI_CS, HIGH);
	  prev_millis = millis();
  }

  if ( ! z80bus.HALT() ) {
    Serial.println("Halted.");
    if ( z80bus.DMA_mode() ) {
      Serial.println("Entered DMA mode.");
      //lcdt.print(0,0,"DMA mode.");
      Serial.println("Memory dump...");
      z80bus.DMA_address(0);
      z80bus.DMA_read(dma_buff);
      dump(dma_buff, 0x100, 0);
      //z80bus.DMA_address(0x100);
      //z80bus.DMA_read(dma_buff);
      //dump(dma_buff, 0x100, 0x100);

      Serial.println("Exit to I/O Controller mode.");
      z80bus.IOC_mode();
    }
    z80bus.clock_stop();
    while (true);
  }
}
