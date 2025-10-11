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
File rootpath;

struct SD_DRIVE {
  static const uint8_t num_of_drives_limit = 8;
  uint8_t num_of_drives;
  char dsknames[num_of_drives_limit][16];

  SD_DRIVE() {
    num_of_drives = 0;
  }

  boolean add_dsk_name(const char * dskname) {
    if ( num_of_drives + 1 < num_of_drives_limit ) {
      strncpy(dsknames[num_of_drives], dskname, 16);
      dsknames[num_of_drives][15] = (char) 0;
      ++num_of_drives;
      return true;
    }
    return false;
  }

  boolean drive_full() const {
    return ! (num_of_drives < num_of_drives_limit) ;
  }
} drives;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }

  Serial.println("initialization done.");

  rootpath = SD.open("/");
  if (! rootpath) {
    Serial.println("error opening search rootpath.");
    return;
  }
  Serial.print("Search for .DSK file in ");
  Serial.println(rootpath.name());

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File entry;
  while (( entry =  rootpath.openNextFile()) == true) {
    String ename = entry.name();
    if (! entry.isDirectory() and ename[0] != '.' and ename[0] != '_') {
      if ( ename.endsWith(".DSK") ) {
        drives.add_dsk_name( (rootpath.name() + ename).c_str());
        //Serial.println(ename);
      }
    }
    entry.close();
    if ( drives.drive_full() ) break;
  }

  Serial.print(drives.num_of_drives);
  Serial.println(" drives.");
  for(uint8_t dno = 0; dno < drives.num_of_drives; ++dno) {
    Serial.println(drives.dsknames[dno]);
    File dskfile = SD.open(drives.dsknames[dno]);
    if ( ! dskfile ) {
      Serial.print("couldn't open ");
      Serial.println(drives.dsknames[dno]);
    } else {
      Serial.println("reading the pages: ");
      Serial.println(dskfile.size(), DEC);
      unsigned long pos;
      for (uint16_t page = 0; page < 1; ++page) {
        pos = page;
        pos <<= 7 ;
        if ( pos < dskfile.size() ) {
          dskfile.seek(pos);
          for(unsigned long offset = 0; offset < 256 ; ++offset ) {
            if ( pos + offset >= dskfile.size() )
              break;
            byte b = dskfile.read();
            Serial.print(b>>8&0x0f, HEX);
            Serial.print(b&0x0f, HEX);
            Serial.print(' ');
            if ( (offset & 0x0f) == 0x0f) {
              Serial.println();
            }
            if ( (offset & 0xff) == 0xff)  {
              Serial.println();
            }
          }
        }
      }
    }
    dskfile.close();
  }
}

void loop() {
}
