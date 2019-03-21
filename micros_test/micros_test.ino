long counter, count_min;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  count_min = 1000;
}

void loop() {
  // put your main code here, to run repeatedly:
  long diff = micros() - counter;
  counter = micros();
  if ( diff < count_min ) {
    count_min = diff;
    Serial.println(count_min);
  }
  
}
