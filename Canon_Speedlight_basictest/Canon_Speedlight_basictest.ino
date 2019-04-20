
#include <Timer1.h>

const int PIN_XBAR = 2; // get HIGH when X/SYNC goes LOW
const int PIN_Y = 3;    // HIGH enables Quench LOW, LOW yields Quench HIGH
const int PIN_Q = 6;    // Quench monitor
const int PIN_QBAR = 4; // ~Quench monitor
const int PIN_LED = 13; // Monitor LED/ imitate flash
const int PIN_READY = 5; // flash READY

const int PIN_DIAL0 = 14;
const int PIN_DIAL1 = 15;
const int PIN_DIAL2 = 16;
const int PIN_DIAL3 = 17;

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
  2520,
  2680,
  2880,
  3200,
  3700,
  4600,
  6400, //5860,
  9900,
  0, 0, 0, 0, 
  };

volatile uint16_t quench_ticks = 0;

void show_state(int stat) {
  Serial.print("XBAR = ");
  Serial.print(stat>>PIN_XBAR & 1);
  Serial.print(" Y = ");
  Serial.print(PORTD>>PIN_Y & 1);
  Serial.print(" Q = ");
  Serial.print(stat>>PIN_Q & 1);
  Serial.println();
}

int last_dial_value;

int dial_value() {
  return 0x0f & ~(digitalRead(PIN_DIAL0) | digitalRead(PIN_DIAL1)<<1 | digitalRead(PIN_DIAL2)<<2 | digitalRead(PIN_DIAL3)<<3 );
}

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_XBAR, INPUT);
  pinMode(PIN_Y, OUTPUT); digitalWrite(PIN_Y, LOW);
  pinMode(PIN_Q, INPUT);
  pinMode(PIN_READY, INPUT_PULLUP);

  pinMode(PIN_LED, OUTPUT); digitalWrite(PIN_LED, LOW);

  pinMode(PIN_DIAL0, INPUT_PULLUP);
  pinMode(PIN_DIAL1, INPUT_PULLUP);
  pinMode(PIN_DIAL2, INPUT_PULLUP);
  pinMode(PIN_DIAL3, INPUT_PULLUP);

  attachInterrupt(0, X_start, RISING);
  last_dial_value = min(dial_value(), 7);
  quench_ticks = Q_ticks[last_dial_value];
  Timer1.mode_ctc(quench_ticks);

//  blink_state(dial_state+1);
  Serial.begin(19200);
  Serial.print("dial = ");
  Serial.print(last_dial_value);
  Serial.print(" ticks = ");
  Serial.print(quench_ticks);
  Serial.println();
}

void X_start() {
  cli();
  Timer1.mode_ctc(quench_ticks);
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
  if ( (newval= min(dial_value(),7)) != last_dial_value) {
    last_dial_value = newval;
    quench_ticks = Q_ticks[last_dial_value];
    //blink_state(dial_state+1);
    //Timer1.mode_ctc(quench_ticks);
    digitalWrite(PIN_LED, HIGH);
    delay(((long)last_dial_value + 1)<<3);
    digitalWrite(PIN_LED, LOW);
    Serial.print("dial = ");
    Serial.print(last_dial_value);
    Serial.print(" ticks = ");
    Serial.print(quench_ticks);
    Serial.println();
  }
}
