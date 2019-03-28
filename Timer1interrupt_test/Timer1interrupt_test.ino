/*
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL  // CPU clock in Hertz
*/

class atmega328_Timer1 {
    unsigned int tcnt1_start;
  
    void init() {
      TCCR1A = 0;
      TCCR1B = 0;      
    }
    
  public:

    enum {
      CS_STOP = 0,
      CS_DIV1 = (1<<CS10),
      CS_DIV8 = (1<<CS11),
      CS_DIV64 = (1<<CS11) | (1<<CS10),
      CS_DIV256 = (1<<CS12),
      CS_DIV1024 = (1<<CS12) | (1<<CS10),
      CS_EXTT1_FALLING = (1<<CS12) | (1<<CS10),
      CS_EXTT1_RISING = (1<<CS12) | (1<<CS10) | (1<<CS10),
    };

    atmega328_Timer1() {
      tcnt1_start = 0;
    }

    unsigned int countdown_start() { return tcnt1_start; }
    
    void mode_normal(unsigned char prescaler_val, unsigned int countup_from) {
      init();
      TCCR1B |= prescaler_val;
      TIMSK1 |= (1<<TOIE1);
      TIMSK1 &= ~((1<<OCIE1A) | (1<<OCIE1B));
      
      tcnt1_start = countup_from;
      TCNT1 = tcnt1_start;
    }

    void mode_ctc(unsigned char prescaler_val, unsigned int count_limit1, unsigned int count_limit2 = 0) {
      init();
      TCCR1B |= prescaler_val | (1<<WGM12);
      OCR1A = count_limit1;
      OCR1B = count_limit2;
      TIMSK1 &= ~(1<<TOIE1);
      TIMSK1 |= (1<<OCIE1A);
      if ( count_limit2 != 0 )
        TIMSK1 |= (1<<OCIE1B);
    }
};

atmega328_Timer1 Timer1;

void setup() {
  // put your setup code here, to run once:

  //  setup Timer1
  Timer1.mode_ctc(Timer1.CS_DIV1024, 31250);

  Serial.begin(19200);
}

ISR(TIMER1_OVF_vect) {
  Serial.println(millis());
  TCNT1 = Timer1.countdown_start();
}

ISR(TIMER1_COMPA_vect) {
  Serial.print("from OCR1A ");  
  Serial.println(millis());
}

ISR(TIMER1_COMPB_vect) {
  Serial.print("from OCR1B ");  
  Serial.println(millis());
}

void loop() {
  // put your main code here, to run repeatedly:

}
