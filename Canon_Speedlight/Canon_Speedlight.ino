
#include <util/delay.h>

enum {
  PIN_X = 3, // X sync
  PIN_Q = 4, // monitor for Q
  PIN_TEST = 5,
  PIN_RDY = 6,  // HIGH while charging
  PIN_AID = 7,
  PIN_LED = 13,
};


int dial_val;
volatile byte state = 0, newstate;
volatile long swatch0, swatch1;
volatile byte state0, state1;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_X,INPUT_PULLUP);
//  pinMode(PIN_XBAR,INPUT);
  pinMode(PIN_Q,OUTPUT); digitalWrite(PIN_Q, HIGH);
  pinMode(PIN_TEST,INPUT_PULLUP);
  pinMode(PIN_RDY,INPUT_PULLUP);
  pinMode(PIN_AID,INPUT_PULLUP);  
  pinMode(PIN_LED,OUTPUT); digitalWrite(PIN_LED,LOW); 

  attachInterrupt(1, X_start, FALLING);
  /*
  state = PIND & 0xFC;
  delayMicroseconds(100);
  */
  Serial.begin(115200);
  /*
  Serial.print(millis()); Serial.print(" "); Serial.println(state>>2, BIN);
  digitalWrite(PIN_QY, LOW);
  delayMicroseconds(100);
  state = PIND & 0xFC;
  Serial.print(millis()); Serial.print(" "); Serial.println(state>>2, BIN);
  digitalWrite(PIN_QY, HIGH);
  delayMicroseconds(100);
  state = PIND & 0xFC;
  Serial.print(millis()); Serial.print(" "); Serial.println(state>>2, BIN);
  */
}


void X_start() {
  digitalWrite(PIN_Q, LOW);
  //PORTD &= ~((byte)1<<PIN_Q);
  _delay_us(500);
  digitalWrite(PIN_Q, HIGH);
  //PORTD |= ((byte)1<<PIN_Q);
}

void loop() {
  // put your main code here, to run repeatedly:

  newstate = (PIND & 0xFC);
  if ( state != newstate ) {
    Serial.print(millis());
    Serial.print(": ");
    Serial.print(state, BIN);
    Serial.print( " -> ");
    Serial.print(newstate, BIN);
    state = newstate;
    if ( state>>PIN_X & 1 ) {
      Serial.print(" X_HIGH ");
    } else {
      Serial.print(" X_LOW  ");
    }
    if ( state>>PIN_AID & 1 ) {
      Serial.print("AID_HIGH ");
    } else {
      Serial.print("AID_LOW  ");
    }
    Serial.println();
    digitalWrite(PIN_LED, digitalRead(PIN_RDY) == LOW);
  }

}
