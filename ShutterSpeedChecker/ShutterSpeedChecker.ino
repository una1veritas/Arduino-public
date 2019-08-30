volatile int state;
long int swatch;
void setup() {
  // put your setup code here, to run once:

  state = 0;
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:

  if ( state == 0 && analogRead(0) < 333 ) {
    state = 1;
  } else if ( state == 1 &&  analogRead(0) > 667 ) {
    swatch = micros();
    state = 2;
  } else if ( state == 2 && analogRead(0) < 333 ) {
    swatch = micros() - swatch;
    Serial.println(swatch);
    state = 0;
  }

}
