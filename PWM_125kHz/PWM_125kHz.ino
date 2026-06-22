/*
  // Set Pin 3 as an output
  pinMode(3, OUTPUT); 

  // Fast PWM Mode with OCR2A as TOP
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20); 

  // Set the TOP limit for 125 kHz (16MHz / 1 / 125000) - 1
  OCR2A = 127; 

  // Set Duty Cycle (0 to 127)
  // Example: 50% duty cycle
  OCR2B = 64; 
  */

  /*
    // Set Pin 9 as an output
  pinMode(9, OUTPUT); 

  // Set up Timer 1 for Fast PWM
  TCCR1A = _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

  // Set TOP limit for 125 kHz
  ICR1 = 127; 

  // Set Duty Cycle (0 to 127)
  OCR1A = 64; // 50% duty cycle
  */

enum CS2x_CLK_SEL_BITS {
  // without shift
  CLK_STOP = 0,
  CLK_T2S_1 = 1,
  CLK_T2S_8 = 2,
  CLK_T2S_32 = 3,
  CLK_T2S_64 = 4,
  CLK_T2S_128 = 5,
  CLK_T2S_256 = 6, 
  CLK_T2S_1024 = 7,
};

// global 
volatile unsigned long T1Counter;
volatile bool counterUpdated;

long led_lit_millis;

ISR(TIMER1_OVF_vect) {
  T1Counter += TCNT1;
  TCNT1 = 0;
}

void setup() {
  pinMode(5, INPUT);
  TCCR1A = 0;
  // ICES1 for falling edge, 
  //TCCR1B =  (1 << ICNC1)  | (0 << ICES1) ;// | (1 << CS11) | (1 << CS10);
  TCCR1B =  (1 << CS12) | (1 << CS11) | (0 << CS10);
  TCNT1 = 0;
  TIMSK1 |= (1 << TOIE1);
  sei();

  T1Counter = 0;

  Serial.begin(115200);

  // digitalWrite(13, LOW);
  pinMode(13, INPUT); 

  // HV boost converter
  pinMode(3, OUTPUT); 

  // put your setup code here, to run once:
  // Set Pin 3 / OC2B as an output
  // Fast PWM Mode with OCR2A as TOP
  TCCR2A = _BV(COM2B1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | CLK_T2S_8; // _BV(CS21) | _BV(CS20); 

  // Set the TOP limit for 125 kHz (16MHz / 1 / 125000) - 1
  const unsigned int PERIOD = 39;
  OCR2A = PERIOD; //127;  

  // Set Duty Cycle (0 to 127)
  // Example: 50% duty cycle
  OCR2B = PERIOD*2/3; // 1mH 1.1ohm , 64 -> 220V, 92 -> 300v ;

}

void loop() {
  if ( TCNT1 > 0 ) {
    led_lit_millis = millis();
    T1Counter += TCNT1;
    TCNT1 = 0;
    Serial.println(T1Counter, DEC);
    counterUpdated = false;
    
  }
  // put your main code here, to run repeatedly:

  if ( millis() - led_lit_millis > 10000 ) {
    Serial.print("A2 = "); Serial.println(analogRead(A2));
    led_lit_millis = millis();
  }
}
