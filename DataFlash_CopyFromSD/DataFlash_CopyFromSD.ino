#include <SPI.h>
#include "SD_SPI.h"
//#include "Des.h"
#include "TextStream.h"
#include "DataFlash_SPI.h"

// On the Ethernet Shield, CS is pin 4. 
const int chipSelect = 4;
SDClass SD(chipSelect);

const char * datafname = "keys.txt";
//const char * dbfname = "keys.dbn";

TextStream mon(Serial);

//SPISRAM sram(SRAM_CS, SPISRAM::BUS_MBits); // CS pin
const int FLASH_CS = 5;
DataFlash flash(5);

byte buf[256];

void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial);
  //delay(1000);

  SPI.begin();  //pinMode(10, OUTPUT);

  Serial.print("Initializing SD card... ");
  SD.begin() || (Serial.println("Card failed, or not present") );
  Serial.println("card initialized.");

  flash.begin();
  Serial.println("Flash initialized.");

  File datafile = SD.open(datafname);
  if ( !datafile ) Serial.println("Failed to open data file.");
  //  File dbfile = SD.open(dbfname, FILE_WRITE);
  //  if ( !dbfile ) Serial.println("Failed to open db file.");

  long swatch = millis();
  long count = 1450;
  if (datafile) {
    count = 0;
    buf[0] = 0;
    TextStream rw(datafile);
    while (rw.available()) {
      // seems to wait for around 35 millisec for the first char ...
      if ( rw.concateLine((char*) buf, 127, 50) || (datafile.size() == datafile.position()) ) {
        if ( buf[0] == '#' || strlen((char*)buf) == 0 ) {
          buf[0] = 0;
          continue;
        }
        buf[15] = 0;
        if ( count < 4 ) {
          Serial.print(count);
          Serial.print(" ");
          Serial.println((char*)buf);
        } 
        else {
          if ( (count % 100) == 0 ) Serial.print('.');
        }
        flash.write(count*15, buf, 15);
        count++;
        buf[0] = 0;
      }
    }
    flash.flush();
    datafile.close();
    //    dbfile.close();
    Serial.println();
    Serial.println(millis() - swatch);
    Serial.println("Loading data finished.");
    Serial.println();
  }  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(datafname);
  } 

  Serial.println("Now reading data from Flash memory.");
  swatch = millis();
  for(long i = 0; i < count; i++) {
    flash.read(i*15, buf, 15);
    buf[15] = 0;
    //if ( i <  5 || i > count - 12 ) {
      Serial.print(i);
      Serial.print(" ");
      Serial.print((char*) buf);
      Serial.println();
    //}
  }
  Serial.println(millis() - swatch);
  Serial.println("Dumping ");
  Serial.print(count);
  Serial.println(" x 15 bytes data finished.");
  Serial.println();

  swatch = millis();
  Serial.println("Search by type, division, pid and issue no.");
  int pos = bsearch(count, "16	S	82541854	1");
  Serial.print(millis() - swatch);
  Serial.println(" msec.");
  Serial.println(pos);

  halt();
  delay(500);

}

void loop()
{
}

boolean halt() {
  for(;;);
  return true;
}

int bsearch(long limit, char id[]) {
  /*
  for(int i = 0; i < 11; i++) {
   Serial.print(id[i], HEX);
   Serial.print(' ');
   }
   Serial.println();
   Serial.println();
   */
  long ib = 0;
  long ie = limit;
  long ix;
  while (ie > ib) {
    ix = (ib+ie)/2;
    flash.read(ix * 15, buf, 15);
    int diff = memcmp(buf, id, 15);
    /*
    r.printOn(Serial);
     Serial.print(ix);
     Serial.print(": ");
     for(int i = 0; i < 11; i++) {
     Serial.print(r.rawbytes[i], HEX);
     Serial.print(' ');
     }
     Serial.print(" :");
     Serial.print(diff);
     Serial.println();
     Serial.println();
     */
    if ( diff == 0 ) {
      return ix;
    }
    if ( diff > 0 ) {
      ie = ix;
    } 
    else {
      ib = ix + 1;
    }
  }
  return -1;
}





