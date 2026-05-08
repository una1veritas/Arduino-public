void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  Serial.println();
  Serial.println("Start");
  volatile uint8_t & ioport = PORTB;
  unsigned long swatch = millis();
  unsigned long uswatch = micros();
  for(long i = 0 ; i < 100000; ++i) {
    ioport |= 1<<5;
    ioport ^= 1<<5;
    //ioport ^= 1<<5;
  }
  uswatch = micros() - uswatch;
  swatch = millis() - swatch;
  Serial.println("Stop");
  Serial.println(uswatch);
  Serial.println(double(uswatch) / 100000);
  Serial.println(swatch);
  Serial.println(double(swatch) / 100000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
