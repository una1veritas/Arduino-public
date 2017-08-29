
void data_out(uint8_t val) {
  DDRC |= 0x0f;
  PORTC &= ~0x0f;
  PORTC |= val & 0x0f;
}

uint8_t data_in() {
  uint8_t val;
  DDRC &= 0xf0;
  val = PORTC;
  return val & 0x0f;
}

const uint8_t RAS = 2;
const uint8_t CAS = 3;
const uint8_t OE = 4;
const uint8_t WE = 5;


void setup() {
  Serial.begin(19200);
  Serial.println("Hi there.");

  pinMode(RAS, OUTPUT);
  digitalWrite(RAS, HIGH);
  pinMode(CAS, OUTPUT);
  digitalWrite(CAS, HIGH);
  pinMode(OE, OUTPUT);
  digitalWrite(OE, HIGH);
  pinMode(WE, OUTPUT);
  digitalWrite(WE, HIGH);
}

void loop() {
  Serial.println(millis());
  delay(1000);
}

