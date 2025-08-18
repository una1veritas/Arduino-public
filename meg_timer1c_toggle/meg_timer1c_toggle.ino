void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  //digitalWrite(13, LOW);
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  TCCR1A = (B01<<COM1C0);// OC1C toggle com1C1 -- 0, com1C0 -- 1. wgm13:0 = B0100 
  TCCR1B = (B01<<WGM12) | (B101<<CS10); // ctc mode, clock 1/1024
  OCR1C = 60000 - 1;
  
  sei();

  while (1) {}
}

void loop() {
  // put your main code here, to run repeatedly:

}
