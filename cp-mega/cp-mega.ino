#include <SPI.h>
#include <SD.h>

#include <cpu_8080.h>

const int chipSelect = 4;

void setup() {

  uart_init();
  sram_init();
  sdcard_init();

  uart_putsln("\r\nbooting CP/Mega88 done.");

  mem_clr();
  mem_chk();

  char rc = sdcard_open();
  if (rc >= 0) 
    uart_putsln("SDC: ok");
  else {
    uart_puts("SDC: err(");
    uart_puthex(-rc);
    uart_putsln(")");
  }

  rc = fat_init();
  if (rc >= 0) {
    uart_puts("FAT: ");
    if ((4 == rc) || (6 == rc)) uart_puts("FAT16");
    else if (0xb == rc) uart_puts("FAT32");
    else uart_puthex(rc);
    uart_putsln("");
    sd_fat = 1;
  } else {
    uart_puts("FAT: ");
    uart_puthex(-rc);
    uart_putsln("");
    sd_fat = 0;
  }

  work.load_8 = &sram_read;
  work.store_8 = &sram_write;
  work.in = &in;
  work.out = &out;

  if (0x88 != eeprom_read(0)) {
    uart_putsln("EEPROM: init");
    int i;
    for (i = 0; i < 256; i++) eeprom_write(i, 0);
    eeprom_write(0, 0x88);
  } else {
    uart_putsln("EEPROM: load");
    if (0x88 == eeprom_read(16)) {
      char buf[8 + 1 + 3 + 1];
      eeprom_read_string(17, buf);
      if (0 != sd_fat) mount(buf);
    }
    if (0x88 == eeprom_read(8)) boot();
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  prompt();
}
