
#include <Timer1.h>

const int PIN_XBAR = 2; // get HIGH when X/SYNC goes LOW
const int PIN_Y = 3;    // HIGH enables Quench LOW, LOW yields Quench HIGH
const int PIN_Q = 6;    // Quench monitor
const int PIN_QBAR = 4; // ~Quench monitor
const int PIN_LED = 13; // Monitor LED/ imitate flash

const int ANALOG_DIAL = 0;

/*
1020 us 約1/980 秒： M 1/1 発光（FULL）
901 us 約1/1110 秒： M 1/2 発光
387 us 約1/2580 秒： M 1/4 発光
194 us 約1/5160 秒： M 1/8 発光
112 us 約1/8890 秒： M 1/16 発光
74 us 約1/13470 秒： M 1/32 発光
53 us 約1/18820 秒： M 1/64 発光
41 us 約1/24250 秒： M 1/128 発光
32.4 us 約1/30820 秒： M 1/256 発光
 */

const uint16_t Q_ticks[] = {
  4800, 3072, 2800, 2600, 2400, 2000, 1000, 1000, 0, 0, 0,
};

void show_state(int stat) {
  Serial.print("XBAR = ");
  Serial.print(stat>>PIN_XBAR & 1);
  Serial.print(" Y = ");
  Serial.print(PORTD>>PIN_Y & 1);
  Serial.print(" Q = ");
  Serial.print(stat>>PIN_Q & 1);
  Serial.println();
}

int dial_state;

int dial_value() {
  static int past_value;
  int current_value = analogRead(ANALOG_DIAL)>>2;
  if ( current_value - 1 > past_value ) {
    past_value = current_value - 1;
  } else if ( current_value + 1 < past_value ) {
    past_value = current_value + 1;
  }
  return (past_value < 1 ? 1 : (past_value > 159 ? 159 : past_value))-1;    
}

void blink_state(int val) {
  for(int i = 0; i < val/5; ++i) {
    digitalWrite(PIN_LED, HIGH);
    delay(200);
    digitalWrite(PIN_LED, LOW);
    delay(200);
  }
  for(int i = 0; i < val % 5; ++i) {
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(100);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_XBAR, INPUT);
  pinMode(PIN_Y, OUTPUT); digitalWrite(PIN_Y, LOW);
  pinMode(PIN_Q, INPUT);
  pinMode(PIN_LED, OUTPUT); digitalWrite(PIN_LED, LOW);

  attachInterrupt(0, X_start, RISING);

  dial_state = dial_value()>>4;
  blink_state(dial_state+1);
  Timer1.mode_ctc(Q_ticks[dial_state]);
  Serial.begin(19200);
  Serial.println(dial_state);
}

void X_start() {
  cli();
  Timer1.start(Timer1.CS_DIV64); // DIV64 = 64 clk (4 usec.)
  digitalWrite(PIN_Y, HIGH);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  cli();
  digitalWrite(PIN_Y, LOW);
  Timer1.stop();
  sei();
}

void loop() {
  int newval;
  // put your main code here, to run repeatedly:
  if ( (newval= dial_value()>>4) != dial_state) {
    dial_state = newval;
    blink_state(dial_state+1);
    Timer1.mode_ctc(Q_ticks[dial_state]<<1);
    Serial.println(dial_state);
  }
}
