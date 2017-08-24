

const byte LATCH_ADDRL = 1<<2;
const byte LATCH_ADDRH = 1<<3;
const byte SRAM_CS = 1<<5;
const byte SRAM_OE = 1<<6;
const byte SRAM_WE = 1<<7;

const byte SRAM_A16 = 1<<0;

uint16_t addr = 0;

void write(uint16_t addr, uint8_t val) {
  
  PORTC = addr & 0xff;
  PORTD &= ~LATCH_ADDRL;
  delay(100);
  PORTD |= LATCH_ADDRL;
  PORTC = addr>>8 & 0xff;
  PORTD &= ~LATCH_ADDRH;
  delay(100);
  PORTD |= LATCH_ADDRH;

  delay(1000);
  PORTC = val;
  PORTD &= ~SRAM_WE;
  delay(1000);
  PORTD |= SRAM_WE;

}

byte read(uint16_t addr) {
  uint8_t val;
  
  PORTC = addr & 0xff;
  PORTD &= ~LATCH_ADDRL;
  delay(100);
  PORTD |= LATCH_ADDRL;
  PORTC = addr>>8 & 0xff;
  PORTD &= ~LATCH_ADDRH;
  delay(100);
  PORTD |= LATCH_ADDRH;

  DDRC = 0x00; // input
  PORTC = 0xff; // pull-up cancel
  PORTD &= ~SRAM_OE; // output value
  __asm__ __volatile__ ("nop");
  val = PINC;
  Serial.println(val, BIN);
  delay(1000);
  PORTD |= SRAM_OE;
  DDRC = 0xff;

  return val;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hello.");

  // IO Mode default
  DDRD |= LATCH_ADDRL | LATCH_ADDRH | SRAM_CS | SRAM_OE | SRAM_WE;
  DDRB |= SRAM_A16;
  // ADDRESS Bus default
  DDRC = 0xff;

  // default value
  PORTD |= LATCH_ADDRL | LATCH_ADDRH | SRAM_CS | SRAM_OE | SRAM_WE;
  DDRB &= ~SRAM_A16;
}

void loop() {
  // put your main code here, to run repeatedly:
  byte val = millis()/1000; //random('0','Z');

  PORTD &= ~SRAM_CS;
  Serial.print("addr: ");
  Serial.println(addr, HEX);

  Serial.print("write: ");
  Serial.println(val, HEX);
  write(addr, val);

  val = read(addr);
  Serial.print("read: ");
  Serial.println(val, HEX);
  delay(1000);
  PORTD |= SRAM_CS;
  delay(1000);

  addr ++;
  Serial.println();
}
