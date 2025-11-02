/* by sin, Aug, 2025 */
#include <SPI.h>
#include <SD.h>

#include "Z80Bus.h"
#include "displays.h"

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

const int SPI_CS = 53;//latchPin = 53; --- must be controlled by user
//const int SPI_CLK = 52; //clockPin = 52; --- controlled by SPI module.
//const int SPI_COPI = 51; //dataPin = 51; --- controlled by SPI module.

void list_dir(const char * rootpath, int idx = -1, char * filename = NULL);

//uint16_t addr;
//uint8_t data;
char busmode = ' ';
long prev_millis;
char buf[32];
uint8_t dma_buff[256];
File dskfile;
File root;

//DFR7segarray dfr7seg(19, 20, 21);
//const int LCD_RS = 14, LCD_EN = 15, LCD_D4 = 16, LCD_D5 = 17, LCD_D6 = 18, LCD_D7 = 19;
//LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// boot/ipl source
enum BOOT_PROGRAM {
  DMA_IPL = 0,
  DISK_BOOTSECTOR,
} boot_media;

void setup() {
  // put your setup code here, to run once:
  //lcdt.print(0,0, "System Starting.");

  Serial.begin(115200);
  while (! Serial) {}
  Serial.println("Z80 system stareding. ");

  boot_media = DMA_IPL;
  // microSD disk/file, SPI 
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);
  // SPI.setClockDivider(SPI_CLOCK_DIV4);  -- div4 seems to be the default clock divider value
  SPI.begin();
  if (! SD.begin(SS) ) {
	  Serial.println("Failed to initialize SD card interface.");
  } else {
    Serial.println("--- root dir of SD ---");
    list_dir("/");
    Serial.println("----------------------");
    
    char filename[16];
    list_dir("/", 15, filename);
    Serial.println(filename);
    z80bus.fdc.drive().dskfile = SD.open(filename);
    if (!z80bus.fdc.drive().dskfile) {
	    Serial.print("Failed to open path/file ");
      Serial.println(filename);
    } else {
      Serial.print("Trying to boot from ");
      Serial.println(filename);
      //boot_media = DISK_BOOTSECTOR;
    }
  }

  //dfr7seg.clear();
  // nop test
  //z80bus.mem_disable();
  z80bus.clock_mode_select(4); 
  z80bus.clock_start();
  Serial.println("Reseting Z80...");
  z80bus.cpu_reset();

  if (! z80bus.mem_bus_DMA_mode() ) {
    //lcdt.print(0,0,"DMA mode failed.");
    while (true) ;
  } else {
    Serial.println("Entered DMA mode.");

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
    if ( boot_media == DMA_IPL) {
      uint8_t res;
      res = z80bus.DMA_progmem_load(Z80Bus::boot_0000, 0x0000, 256);
      res |= z80bus.DMA_progmem_load(Z80Bus::mon_0100, 0x0100, 256*3);
      if (res != 0) {
        Serial.println("Something going wrong w/ sram/IPL read & write!");
      } else {
        Serial.println("Transfer completed.");
      }
    } else if (boot_media == DISK_BOOTSECTOR ) {
      z80bus.fdc.drive().dskfile.seek(0);
      for(unsigned long pos = 0; pos < 256; ++pos) {
        z80bus.ram_write(pos, z80bus.fdc.drive().dskfile.read());
      }
      /*
      z80bus.dma.set_block_size(0);
      z80bus.dma.transfer_mode = z80bus.dma.WRITE_RAM;
      for(unsigned long sec = 0; sec < 2; ++sec) {
        z80bus.DMA_address(sec * z80bus.dma.block_size());
        z80bus.fdc.setup_read(0, 0, sec);
        z80bus.FDC_operate(dma_buff);
        z80bus.DMA_exec(dma_buff);
      }
      */
    }
    Serial.println("Exit to Z80 mem bus mode.");
    z80bus.mem_bus_Z80_mode();
  }
  Serial.println("_RESET goes HIGH.");
  z80bus.RESET(HIGH);
  //lcdt.clear();
  //lcdt.print(0,0,"Z80 starts.");
  prev_millis = millis();
}

void loop() {
  uint32_t val;
  uint16_t addr;
  uint8_t data, bus_mode;

  //z80bus.clock_wait_rising_edge();
   if ( !z80bus.MREQ() ) {/*
    if ( ! z80bus.RD() ) {
      //addr = z80bus.address_bus16_get();
      //z80bus.clock_wait_rising_edge();
      //data = z80bus.data_bus_get();
      //val = z80bus.mem_rw();
      bus_mode = 'r';      
    } else if ( ! z80bus.WR() ) {
      //addr = z80bus.address_bus16_get();
      //z80bus.clock_wait_rising_edge();
      //data = z80bus.data_bus_get();
      //val = z80bus.mem_rw();
      bus_mode = 'u';
    } // else RFSH
    */
  } else if ( !z80bus.IORQ() ) {
    //if ( ! z80bus.RD() ) {
      //val =
    z80bus.io_rw();
    //} else if ( ! z80bus.WR() ) {
    //  val = z80bus.io_rw();
    //} 
    //data = ((uint8_t *) &val)[0];
    //bus_mode = ((uint8_t *) &val)[1];
    //addr = ((uint16_t *) &val)[1];
  }

/*
  if (z80bus.clock_mode_current() < 4) {
    dfr7seg.show_digits(addr, data, bus_mode);
  }

  if ( ! z80bus.HALT() ) {
    Serial.println("Halted.");
    z80bus.clock_stop();
    Serial.println("Z80 Clock stopped.");
    while (true);
  }
  */
}

void list_dir(const char * rootpath, int idx = -1, char * filename = NULL) {
  int fidx = 0;
  File entry;
  char buf[16];
  root = SD.open(rootpath);
  root.rewindDirectory();
  for(fidx = 0; (entry =  root.openNextFile()) ; ++fidx, entry.close()) {
    if ( ! String(entry.name()).startsWith("_") and ! entry.isDirectory() ) {
      if ( idx == -1 ) {
        Serial.print(fidx);
        Serial.print("\t");
        strcpy(buf, entry.name());
        Serial.print(buf);
        // files have sizes, directories do not
        Serial.print("\t\t");
        Serial.println(entry.size(), DEC);
      } else if (idx == fidx) {
        strcpy(filename, entry.name());
      }
    }
  }
  root.close();
}