
void start_OC1A(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}


void setup() {
  // put your setup code here, to run once:
  start_OC1A(5, 1000);
  pinMode(9, OUTPUT); // PB1

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(8, LOW);
  delay(500);
  digitalWrite(8, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}
