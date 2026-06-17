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

void setup() {
  // put your setup code here, to run once:
  // Set Pin 3 / OC2B as an output
  digitalWrite(123, HIGH);
  pinMode(13, INPUT); 

  pinMode(3, OUTPUT); 

  // Fast PWM Mode with OCR2A as TOP
  TCCR2A = _BV(COM2B1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20); 

  // Set the TOP limit for 125 kHz (16MHz / 1 / 125000) - 1
  OCR2A = 127; //127;  

  // Set Duty Cycle (0 to 127)
  // Example: 50% duty cycle
  OCR2B = 96;// 1mH 1.1ohm , 64 -> 220V, 92 -> 300v ;

}

void loop() {
  // put your main code here, to run repeatedly:

}
