

void outlow(byte val) {
  DDRC = 0xff;
  DDRD |= 0xC0;
  PORTC = val;
  PORTD &= 0xC0;
  PORTD |= val & 0xC0;

  return val;
}

void outhigh(byte val) {
  DDRD |= 0x38;
  PORTD &= ~0x38;
  PORTD |= (val<<3) & 0x38;
}

byte in(void) {
  byte val;
  DDRC = 0x00;
  DDRD &= 0x3f;
  val = PINC & 0x3f;
  val |= PIND & 0xC0;
  return val;
}


const byte SRAM_WE = 11;
const byte SRAM_OE = 10;
const byte SRAM_CS = 9;
const byte LATCH = 8;
void latch() {
  digitalWrite(LATCH, LOW);
  digitalWrite(LATCH, HIGH);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  Serial.println("Hi.");

  pinMode(LATCH, OUTPUT);
  pinMode(SRAM_CS, OUTPUT);
  digitalWrite(SRAM_CS, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t addr = 0;
  byte val = random(1,255);
  Serial.println(val, HEX);
  outlow(addr & 0xff);
  latch();
  outhigh(addr>>8 & 0xff);
  outlow(val);
  digitalWrite(SRAM_WE, LOW);
  digitalWrite(SRAM_WE, HIGH);
  
  val = in();
  Serial.println(val, HEX);
  Serial.println();

  delay(1000);
}
