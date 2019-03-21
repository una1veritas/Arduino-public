
#include <util/delay.h>

enum {
  PIN_X = 3,
  PIN_Q = 4,
  PIN_LED = 13,
};

int dial_val;
volatile int state = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_X,INPUT);
  pinMode(PIN_Q,OUTPUT); digitalWrite(PIN_Q,LOW); 
  pinMode(PIN_LED,OUTPUT); digitalWrite(PIN_LED,LOW); 

  attachInterrupt(0, X_start, FALLING);
  
  state = PIND & 0xc;
  Serial.begin(19200);
  Serial.print(millis()); Serial.print(" "); Serial.println(state, BIN);
}

void X_start() {
  state = PIND & 0xc;
  _delay_us(10);
  PORTD |= (1<<PIN_Q);
  while (digitalRead(PIN_X) == LOW) {}
  PORTD &= ~(1<<PIN_Q);

}

void loop() {
  // put your main code here, to run repeatedly:

  if ( state != (PIND & 0x0c) ) {
    state = (PIND & 0x0c);
    Serial.print(millis()); Serial.print(" "); Serial.println(state, BIN);
  }

}
