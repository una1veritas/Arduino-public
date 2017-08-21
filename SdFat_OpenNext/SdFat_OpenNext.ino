/*
 * Print size, modify date/time, and name for all files in root.
 */
#include <SPI.h>
#include "SdFat.h"
#include "SPISRAM.h"


// SD default chip select pin.
const uint8_t chipSelect = 20;//SS;

// file system object
SdFat sd;

SdFile file;

SPISRAM sram(5,SPISRAM::BUS_MBits);

//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  pinMode(5,OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(18,OUTPUT);
  digitalWrite(18, HIGH);
  pinMode(20,OUTPUT);
  digitalWrite(20, HIGH);
  // Wait for USB Serial 
  while (!Serial) {
    SysCall::yield();
  }
  
  Serial.println("Type any character to start");
  while (!Serial.available()) {
    SysCall::yield();
  }

  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }

  // Open next file in root.  The volume working directory, vwd, is root.
  // Warning, openNext starts at the current position of sd.vwd() so a
  // rewind may be neccessary in your application.
  sram.seek(0);
  sd.vwd()->rewind();
  while (file.openNext(sd.vwd(), O_READ)) {
    file.printFileSize(&sram);
    sram.write(' ');
    file.printModifyDateTime(&sram);
    sram.write(' ');
    file.printName(&sram);
    if (file.isDir()) {
      // Indicate a directory.
      sram.write('/');
    }
    sram.println();
    file.close();
  }
  Serial.println("Done!");
  while ( sram.available() > 0 ) {
    Serial.print((char)sram.read());
  }
  Serial.println();
}
//------------------------------------------------------------------------------
void loop() {}
