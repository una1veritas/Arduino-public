/*
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL  // CPU clock in Hertz
*/

#include <Timer1.h>

void setup() {
  // put your setup code here, to run once:

  //  setup Timer1
  Timer1.mode_ctc(100);
  
  pinMode(13, OUTPUT); digitalWrite(13, LOW);
  pinMode(8, INPUT); 
  Serial.begin(19200);
}

ISR(TIMER1_OVF_vect) {
  Serial.println(millis());
  TCNT1 = Timer1.count_start_val();
}

ISR(TIMER1_COMPA_vect) {
  cli();
  digitalWrite(13, LOW);
  Timer1.stop();
  sei();
  Serial.print("from OCR1A ");
  Serial.println(millis());
}

ISR(TIMER1_COMPB_vect) {
  Serial.print("from OCR1B ");  
  Serial.println(millis());
}

void loop() {
  // put your main code here, to run repeatedly:
    
  if (digitalRead(8) == LOW) {
    while (digitalRead(8) == LOW) {}
    delay(500);
    digitalWrite(13, HIGH);
    Timer1.start(Timer1.CS_DIV256);
  }
}
