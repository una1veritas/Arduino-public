#include "message.h"
//global 
long lastmillis;

message msg("THIS IS NOT A DRILL.");

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  lastmillis = millis();

  //Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( millis() - lastmillis >= 150 ) {
    lastmillis = millis();    
    if ( msg.nextbit() ) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, HIGH);
      delay(50);
      digitalWrite(13, LOW);    
    }
  }
}
