// global
unsigned long passed_seconds;
unsigned long last_millis;
unsigned int count_per_10sec[6] = { 0, 0, 0, 0, 0, 0 };

void setup() {
  pinMode(5, INPUT);
  // put your setup code here, to run once:
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12) | (1 << CS11); // External clock on T1 falling edge (CS10 = 0)
  TCNT1 = 0; // Clear counter
   Serial.begin(115200);

  last_millis = millis();
  passed_seconds = millis() / 1000;
}

void loop() {
  // put your main code here, to run repeatedly:
    unsigned int pulseCount;

  if ( millis() - last_millis >= 1000 ) {
    if ( millis() - last_millis >= 2000 ) {
      Serial.println("error!");
    }
    last_millis += 1000;
    passed_seconds += 1;

    if ( (passed_seconds % 10) == 0) {
      pulseCount = TCNT1;
      TCNT1 = 0;
      passed_seconds %= 60;
      count_per_10sec[passed_seconds / 10] = pulseCount;

      unsigned int sum = 0;
      for(unsigned int i = 0; i < 6; ++i) {
        sum += count_per_10sec[i];
      }
      Serial.print("cpm ");
      Serial.println(sum);
    }
  }
}
