/* OC1C test on Arduino Mega/ATmega2560 */

void start_OC1C(uint8_t presc, uint16_t top) {
  const uint8_t WGM_CTC_OCR1A = B0100;
  const uint8_t COM_TOGGLE = B01;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (COM_TOGGLE << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
  TCCR1B |= (((WGM_CTC_OCR1A>>2) & B11)<< WGM12)  | (presc << CS10);

  sei();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  start_OC1C(5, 4000);
  while (1) {}
}

void loop() {
  // put your main code here, to run repeatedly:

}
