
#include <Timer1.h>

const int PIN_XBAR = 2;
const int PIN_Y = 6;
const int PIN_Q = 7;
const int PIN_LED = 13;

void show_state(int stat) {
  Serial.print("XBAR = ");
  Serial.print(stat>>PIN_XBAR & 1);
  Serial.print(" Y = ");
  Serial.print(PORTD>>PIN_Y & 1);
  Serial.print(" Q = ");
  Serial.print(stat>>PIN_Q & 1);
  Serial.println();
}

volatile int state;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_XBAR, INPUT);
  pinMode(PIN_Y, OUTPUT); digitalWrite(PIN_Y, LOW);
  pinMode(PIN_Q, INPUT);
  pinMode(PIN_LED, OUTPUT); digitalWrite(PIN_LED, LOW);

  attachInterrupt(0, X_start, RISING);

  Timer1.mode_ctc(200);
  
  Serial.begin(19200);
  state = (PIND & (1<<PIN_XBAR | 1<<PIN_Q));
  show_state(state);
}

void X_start() {
  Timer1.start(Timer1.CS_DIV256);
  digitalWrite(PIN_Y, HIGH);
  digitalWrite(PIN_LED, HIGH);  
}

ISR(TIMER1_COMPA_vect) {
  cli();
  Timer1.stop();
  digitalWrite(PIN_Y, LOW);
  digitalWrite(PIN_LED, LOW);  
  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
    
}
