/* Arduino UNO/ATmega328 */

void start_OC1A(uint8_t presc, uint16_t top) {
  const uint8_t WGM_CTC_OCR1A = 4;
  const uint8_t COM_TOGGLE = B01;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (COM_TOGGLE << COM1A0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
  TCCR1B |= (((WGM_CTC_OCR1A>>2) & B11)<< WGM12)  | (presc << CS10);

  sei();
}

void start_OC1B(uint8_t presc, uint16_t top) {
  const uint8_t WGM_CTC_OCR1A = 4;
  const uint8_t COM_TOGGLE = B01;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  // compare output mode
  TCCR1A |= (COM_TOGGLE << COM1B0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
  // Waveform Generation Mode
  TCCR1B |= (((WGM_CTC_OCR1A>>2) & B11)<< WGM12) | (presc << CS10);

  sei();
}


void setup() {
  // put your setup code here, to run once:
  start_OC1B(5, 8000);
  pinMode(10, OUTPUT); // PB2
}

void loop() {
  // put your main code here, to run repeatedly:

}
