
// I/O 1 -- 4 <==> PC 0 -- 3
// A8 <==> PC4
// A9 <==> PC5
// A0 -- A3 <==> PB0 -- PB3
// A4 -- A7 <==> PD4 -- PD7
// ~WE <==> PD3
// ~RAS <==> PD2
// ~CAS <==> PB5 (d13)
// ~OE <==> PB4 (d12)
const uint8_t WE = 1<<3;
const uint8_t RAS = 1<<2;
const uint8_t CAS = 1<<5;
const uint8_t OE = 1<<4;

void addr_out(uint16_t addr) {
  PORTB &= ~0x0f;
  PORTB |= addr & 0x0f;
  PORTD = (PORTD & 0x0f) | (addr & 0xf0);
  addr >>= 8;
  PORTC = (PORTC & ~0x30) | ((addr & 0x03)<<4);
}

uint8_t data_in(void) {
  uint8_t val;
  PORTB &= ~OE;
  val = PINC;
  PORTB |= OE;
  return val & 0x0f;
}

uint8_t read(uint32_t addr) {
  uint8_t val;
  uint16_t row = addr >> 8;
  addr_out(row);
  PORTD &= ~RAS;
  addr <<= 1;
  addr_out(addr & 0xfffe);
  DDRC &= ~0x0f;
  PORTB &= ~CAS;
  val = data_in();
  PORTB |= CAS;
  addr++;
  val <<= 4;
  addr_out(addr & 0xffff);
  PORTB &= ~CAS;
  val |= data_in();
  PORTB |= CAS;
  PORTD |= RAS;
  return val;
}

void data_out(uint8_t val) {
  PORTC &= ~0x0f;
  PORTC |= (val & 0x0f);  
}

void write_early(uint32_t addr, uint8_t val) {
  uint16_t row = addr >> 8;
  addr_out(row);
  PORTD &= ~RAS;
  DDRC |= 0x0f;
  data_out(val);
  addr <<= 1;
  addr_out(addr & 0xfffe);
  PORTB &= ~CAS;
  PORTD &= ~WE;
  PORTB |= CAS;
  PORTD |= WE;
  addr++;
  data_out(val>>4);
  addr_out(addr & 0xffff);
  PORTB &= ~CAS;
  PORTD &= ~WE;
  PORTB |= CAS;
  PORTD |= WE; 
  PORTD |= RAS;
}

void refresh() {
  for(uint32_t i = 0; i < 1024; i++) {
    PORTB &= ~CAS;
    PORTD &= ~RAS;
    PORTB |= CAS;
    PORTD |= RAS;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi there.");

  DDRC |= 0x30;
  DDRB |= ( 0x0f | OE | CAS);
  DDRD |= 0xf0 | RAS | WE;

  PORTB |= (OE | CAS);
  PORTD |= RAS | WE;
}

void loop() {
  // put your main code here, to run repeatedly:0
  uint8_t val;
  refresh();
  val = read(0);
  Serial.print(val, HEX);
  refresh();
  val = millis() / 100;
  Serial.print(" ");
  refresh();
  write_early(0,val);
  Serial.print(val, HEX);
  refresh();
  Serial.println();
}
