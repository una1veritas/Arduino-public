#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <SoftwareSerial.h>
#include <GPS.h>

//SoftwareSerial serial2_3(2,3);
GPS gps(Serial);

const int bufferSize = 128;
char buffer[bufferSize], * bp;
int pos, head;
boolean lineFinished;

union datatoken {
  char str[12];
  long val;
} data[16];

void setup() {
  bool val = 0;
  Serial.begin(4800);
  gps.begin();
  
  Serial.print("GPS started.");
  //
  lineFinished = true;
  pos = head = 0;
}

void loop() {
  unsigned long k;
  char tmp[64];
  int n;
  bool succ;
  
  if ( (k = gps.catchMessage(5000)) ) {
    switch(k) {
      case 0x524d43: // RMC
        succ = gps.readRMC();
        Serial.print("RMC (524D43): ");
        Serial.print("JST = ");
        Serial.print(gps.JST());
        Serial.print("; ");
        if ( succ ) {
          Serial.print(gps.latitude(),5);
          Serial.print(", ");
          Serial.print(gps.longitude(),5);
          Serial.print("; ");
          Serial.print(gps._knots,4);
          Serial.print(", ");
          Serial.print(gps._degree,4);
        }
        Serial.println();
        break;
      case 0x474741: // GGA
        succ = gps.readGGA();
        Serial.print("GGA (474741): ");
        Serial.print("JST = ");
        Serial.print( gps.JST() );
        Serial.print("; ");
        if ( succ ) {
          Serial.print(gps.latitude(),5);
          Serial.print(", ");
          Serial.print(gps.longitude(),5);
          Serial.print("; ");
          Serial.print(gps._altitude,4);
        }
        Serial.println();
        break;
      default:
      
        n = gps.readBytesUntil('\n', tmp, 64);
        tmp[n] = 0;
        Serial.print( char(k>>16&0xff));
        Serial.print( char(k>>8&0xff));
        Serial.print( char(k&0xff));
        Serial.print(" (");
        Serial.print(k, HEX);
        Serial.print("): ");
        Serial.println(tmp);
        break;
    }
  } else {
    Serial.println("Timeout.");
  }
}


