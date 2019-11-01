
#include <Timer1.h>
#include <Wire.h>
#include "CharacterLCD.h"
#include "ST7032i.h"

ST7032i lcd;             // Number of lines and i2c address of the display

volatile uint16_t quench_ticks = 0;
struct controller_status {
  int dial_val;
  bool switch_val;
  bool changed;

  const int power_val_min = 0;
  const int power_val_max = 7;
  
  controller_status() {
      dial_val = 0; // 
      switch_val = false;
      changed = true;
  }
  
} cstat;

const int PIN_XBAR = 2; // get HIGH when X/SYNC goes LOW
const int PIN_Y = 3;    // HIGH enables Quench LOW, LOW yields Quench HIGH
const int PIN_QBAR = 4; // ~Quench monitor
const int PIN_READY = 5; // flash READY
const int PIN_Q = 6;    // Quench monitor

const int PIN_BTN = 7;    // sw

const int PIN_BLUMON = 8;
const int PIN_GRNMON = 9; 

const int PIN_LED = 13; // Monitor LED/ imitate flash

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

const char * dispstr[][2] = {
  "1/128   ", "GN  1.0 ",
  "1/64    ", "GN  1.4 ",
  "1/32    ", "GN  2.0 ",
  "1/16    ", "GN  2.8 ",
  "1/8     ", "GN  4.0 ",
  "1/4     ", "GN  5.6 ",
  "1/2     ", "GN  8.0 ",
  "1/1     ", "GN 11.0 ",
  "", "",
  "", "",
  };

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

  bool sw() {
      return (PIND & (1<<PD7)) != 0;
  }

  int dial() {
    int t = ~PINC & 0x0f;
    return 7-min(t,7);
  }

void display_state() {
  lcd.setCursor(0,0);
  lcd.print(dispstr[cstat.dial_val][0]);
  lcd.setCursor(0,1);
  lcd.print(dispstr[cstat.dial_val][1]);
}

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  lcd.begin();         // Init the display, clears the display 
  lcd.setContrast(30); // 30 for aki module

  DDRD &= ~(1<<PD2 | 1<<PD4 | 1<<PD6 | 1<<PD7); // input (bit clear)
  DDRD |= (1<<PD3); // output: quench enable pin (bit set)
  PORTD |= (1<<PD3 | 1<<PD7);  // output high(quench enable)/pull-up sw

  // blue, green -> d8, d9
  DDRB &= ~(1<<PB0 | 1<<PB1);  // input
  DDRB &= ~(1<<PB5);  // output (led) low
  /*
  pinMode(PIN_XBAR, INPUT);
  pinMode(PIN_Y, OUTPUT); digitalWrite(PIN_Y, LOW);
  pinMode(PIN_Q, INPUT);
  pinMode(PIN_READY, INPUT_PULLUP);
  
  pinMode(PIN_LED, OUTPUT); digitalWrite(PIN_LED, LOW);
*/

  // set pin mode for d14 (dial 0) -- d17 (dial 3) 
  DDRC &= 0xf0;
  PORTC |= 0x0f;

  cstat.dial_val = dial();
  cstat.switch_val = sw();

  attachInterrupt(0, X_start, RISING);
  //last_dial_value = min(dial_value(), 7);
  quench_ticks = Q_ticks[cstat.dial_val];
  Timer1.mode_ctc(quench_ticks);

  display_state();
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
  int new_dial_pos = dial();
  if ( new_dial_pos != cstat.dial_val) {
     cstat.dial_val = new_dial_pos;
     
     quench_ticks = Q_ticks[cstat.dial_val];
     display_state();
  }
  if (PIND & (1<<PD7)) {
    PORTB &= ~(1<<PB5);
  } else {
    PORTB |= (1<<PB5);
  }
  /*
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
  */
}
