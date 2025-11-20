/*
  SD card file dump

  This example shows how to read a file from the SD card using the
  SD library and send it over the serial port.
  Pin numbers reflect the default SPI pins for Uno and Nano models.

  The circuit:
   SD card attached to SPI bus as follows:
 ** SDO - pin 11
 ** SDI - pin 12
 ** CLK - pin 13
 ** CS - depends on your SD card shield or module.
 		Pin 10 used here for consistency with other Arduino examples
   (for MKR Zero SD: SDCARD_SS_PIN)

  created  22 December 2010
  by Limor Fried
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.
*/
#include <SD.h>

const int chipSelect = SS;

unsigned long hex_to_ul(const char * ptr, uint8_t len) {
  char buf[6];
  strncpy(buf, ptr, len);
  buf[len] = 0;
  return strtoul(buf, NULL, 16);
}

uint8_t parse_ihex_line(const char * line) {
  char buf[64];
  //Serial.println(line);
  unsigned int n = strlen(line);
  if ( line[0] != ':' ) {
    Serial.print(line[0], HEX);
    return 1;
  }
  uint32_t base_address = 0;

  uint8_t  byte_count = (uint8_t) hex_to_ul(&line[1], 2);
  uint32_t address = (uint32_t) hex_to_ul(&line[3], 4);
  uint8_t  rec_type = (uint8_t) hex_to_ul(&line[7], 2);
  uint8_t  chksum = hex_to_ul(&line[9+(byte_count<<1)], 2);
  uint8_t sum = byte_count + ((uint8_t *) &address)[0] + ((uint8_t *) &address)[1] 
                + rec_type;
  unsigned long val;
  uint32_t current_address = base_address + address;

  if ( rec_type == 0x00 ) {
    // data bytes
    Serial.print("addr = ");
    Serial.print(current_address, HEX);
    Serial.print(", bytes = ");
    Serial.print(byte_count);
    Serial.print(" : ");
    for (int i = 0; i < byte_count; ++i) {
        uint8_t db = (uint8_t)hex_to_ul(&line[9 + (i << 1)], 2);
        snprintf(buf, 31, "%02X ", db);
        Serial.print(buf);
        sum += db;
        // Store data_byte at current_address + i in your memory structure
    }
    Serial.println();
  } else if ( rec_type == 0x01) {
    // End of File
    Serial.println("End of File Record.\n");
  } else if (rec_type == 0x04) {
    // Extended Linear Address
    uint32_t xaddress_msw = hex_to_ul(&line[9], 4);
    sum += ((uint8_t *) &xaddress_msw)[0] + ((uint8_t *) &xaddress_msw)[1];
    base_address |= (unsigned int) xaddress_msw << 16;
    sum = hex_to_ul(&line[13], 2);
    Serial.print("Extended Linear Address: ");
    Serial.println(base_address, HEX);
  }
  if ( chksum != (sum^0xff) + 1 ) {
    /*
    Serial.print(chksum,HEX);
    Serial.print(", ");
    Serial.print((sum^0xff) + 1,HEX);
    Serial.println(" check sum error.");
    */
    return 255;
  }
  return 0;
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(38400);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);

  Serial.print("Initializing SD card...");

  if ( SD.begin(chipSelect) ) {
    Serial.println("initialization done.");
  } else {
    Serial.println("initialization failed. Things to check:");
    while (true);
  }


  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File sdfile = SD.open("Z80BASIC.HEX");
  if (! sdfile ) {
    Serial.println("Error: failed to open file.");
    while (true);  
  }
  char lbuf[256] = { 0, };
  char bufix = 0;
  String line("");
  int tmp;
  uint8_t c;
  while (sdfile.available()) {
    while (sdfile.available() ) {
      tmp = sdfile.read();
      if ( tmp == -1 ) {
        break;
      }
      c = uint8_t(tmp);
      if ( c == 0x0d ) {
        tmp = sdfile.peek(); // assumes LF or -1
        if ( uint8_t(tmp) = 0x0a ) {
          sdfile.read();
          if ( parse_ihex_line(lbuf) ) {
            Serial.print("'");
            Serial.print(lbuf);
            Serial.print("'");
            Serial.println(" ihex format error.");
          }
          // clear buf
          bufix = 0;
          lbuf[bufix] = 0;
          continue;
        }
      }
      if (! (bufix < 256) ) {
        Serial.print(lbuf);
        // flush buf
        bufix = 0;
        lbuf[bufix] = 0;
      }
      // append to buf
      lbuf[bufix++] = (char) c;
      lbuf[bufix] = (char) 0;
    }
    // buffer is not empty, ended without cr/lf
    if ( bufix != 0 ) {
      Serial.println(lbuf);
    }
    if ( tmp == -1 ) {
      Serial.println("seems read -1 before reaching file end.");
    }
  }

  sdfile.close();
  Serial.println("\nAll done.");
}

void loop() {
}
