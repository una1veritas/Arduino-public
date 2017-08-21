#include <SoftwareSerial.h>

const int stx = 15;
const int srx = 14;

SoftwareSerial sserial(50, 51, true);

void setup() {
  // put your setup code here, to run once:

  pinMode(13, OUTPUT);
  pinMode(4,INPUT);
  pinMode(9,OUTPUT);
  digitalWrite(9, LOW);
  Serial.begin(115200);
  sserial.begin(9600);

  Serial.println("Hi.");
}

void loop() {
  // put your main code here, to run repeatedly:
  int c;
  if (digitalRead(4) == HIGH) {
    digitalWrite(13, HIGH);
    if ( Serial.available() > 0 ) {
      while ( Serial.available() > 0 ) {
        c = Serial.read();
        sserial.write((byte)c);
      }
    }
  } else {
    digitalWrite(13, LOW); 
  }
  
  digitalWrite(9, HIGH);
  if ( sserial.available() > 0 ) {
    while ( sserial.available() > 0 ) {
      c = sserial.read();
      Serial.write((byte)c);
    }
  }
  digitalWrite(9, LOW);

  
}
