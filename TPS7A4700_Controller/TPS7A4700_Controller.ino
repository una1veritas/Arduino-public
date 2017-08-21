
enum VSET_PINS {
  VSET0 = 2,
  VSET1,
  VSET2,
  VSET3,
  VSET4,
  VSET5,
  VSET6,
  VSET7,
  VSET_COUNT
};

const unsigned int vset[] = {
  VSET0,
  VSET1,
  VSET2,
  VSET3,
  VSET4,
  VSET5,
  VSET6,
  VSET7,
};

void setup() {
  
  for (unsigned int pin = VSET0; pin < VSET_COUNT; pin++) {
    pinMode(pin, INPUT);
    digitalWrite(pin, LOW); // Z state
  }
  
  pinMode(13, OUTPUT);
}


void loop() {
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(5000);
}

