#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//#include <SoftwareSerial.h>
#include <GPS.h>

//SoftwareSerial sserial(3,2);
const int PIN_POWER = 5;
GPS gps(Serial2, PIN_POWER);
//const int PIN_EN = 5;
boolean PPSstat;

void setup() {
  bool val = 0;
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(PIN_POWER, OUTPUT);
  digitalWrite(PIN_POWER, HIGH);
  
  if ( gps.begin() ) {
    Serial.println("GPS started.");
  } else {
    Serial.println("GPS start seems failed.");
  }
  //
  delay(2000);
}

void loop() {
  unsigned long k;
  char tmp[64];
  int n;
  bool succ;
  
  
  if ( (k = gps.catchMessage(100)) ) {
    switch(k) {
      
      case 0x524d43: // RMC
        succ = gps.readRMC();
        Serial.print("RMC (524D43): ");
        Serial.print("UTC = ");
        Serial.print(gps.UTC());
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
        Serial.print("UTC = ");
        Serial.print( gps.UTC() );
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
      
      case 0x475341: // GSA
        succ = gps.readGSA();
        Serial.print("GSA (475341): ");
        Serial.print("UTC = ");
        Serial.print(gps.UTC());
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
       Serial.print(char(k>>16&0xff)); 
       Serial.print(char(k>>8&0xff)); 
       Serial.print(char(k&0xff)); 
        gps.readLine('\n',tmp,64);
        Serial.print(": ");
        Serial.println(tmp);
        break;
    }
  }
}


