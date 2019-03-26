const int PIN_S = 5;
const int PIN_Y = 6;
const int PIN_Q = 3;
const int PIN_LED = 13;

volatile int state;
void setup() {
  // put your setup code here, to run once:
//  pinMode(PIN_XB, INPUT);
  pinMode(PIN_S, OUTPUT); digitalWrite(PIN_S, HIGH);
  pinMode(PIN_Y, OUTPUT); digitalWrite(PIN_Y, HIGH);
  pinMode(PIN_Q, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT); digitalWrite(PIN_LED, HIGH);
  Serial.begin(19200);
  state = (PIND & 0xfc);
    Serial.println(state, BIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if ( state != (PIND & 0xfc) ) {
    state = (PIND & 0xfc);
    Serial.println(state, BIN);
  }
  
  digitalWrite(PIN_LED, digitalRead(PIN_Q));
  
}
