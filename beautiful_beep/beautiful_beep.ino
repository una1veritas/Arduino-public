const int buzzerPin = 8; // Connect passive buzzer/speaker here

// global 
unsigned long sound_start, sound_stop;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  sound_stop = millis();
}

void loop() {
  if ( millis() - sound_stop > 5000 ) {
    sound_start = millis();
    tone(buzzerPin, 2400);
  } else if ( millis() - sound_start > 100 ) {
    noTone(buzzerPin);
    sound_stop = millis();
  }
}
