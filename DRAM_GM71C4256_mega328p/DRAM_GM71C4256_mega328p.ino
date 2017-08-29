
// I/O 1 -- 4 <==> PD4 -- PD7
// A0 -- A6 <==> PC0 -- PC5
// A7 -- A8 <==> PB0 -- PB2
// ~RAS <==> PB4 (d12)
// ~CAS <==> PB5 (d13)
// ~OE <==> PD2
// ~WE <==> PD3

#define ADDRL PORTC
#define ADDRH PORTB
#define ADDRH_MASK 0x03

#define DATA PORTD
#define DATA_DDR DDRD
#define DATA_MASK 0xf0

#define ADDRCONT PORTB
#define RAS (1<<4)
#define CAS (1<<5)
#define RWCONT PORTD
#define WE (1<<3)
#define OE (1<<4)

void addr_out(uint16_t addr) {
  ADDRL = addr & 0x3f;
  ADDRH &= ~ADDRH_MASK;
  addr >>= 6;
  ADDRH |= addr & ADDRH_MASK;
}

uint8_t data_in(void) {
  uint8_t val;
  DDRD &= ~DATA_MASK;
  PORTB &= ~OE;
  val = PINC;
  PORTB |= OE;
  return val>>4;
}

uint8_t read(uint32_t addr) {
  uint8_t val;
  addr_out(addr>>9);
  ADDRCONT &= ~RAS;
  addr_out(addr & 0x1ff);
  ADDRCONT &= ~CAS;
  DATA_DDR &= ~DATA_MASK;
  RWCONT &= ~OE;
  val = PIND;
  RWCONT |= OE;
  ADDRCONT |= CAS;
  ADDRCONT |= RAS;
  return val>>4;
}

void write(uint32_t addr, uint8_t val) {
  addr_out(addr>>9);
  ADDRCONT &= ~RAS;
  addr_out(addr & 0x1ff);
  ADDRCONT &= ~CAS;
  DATA_DDR |= DATA_MASK;
  DATA &= ~DATA_MASK;
  DATA |= (val<<4);
  RWCONT &= ~WE;
  RWCONT |= WE;
  ADDRCONT |= CAS;
  ADDRCONT |= RAS;
}

void refresh() {
  for(uint32_t i = 0; i < 512; i++) {
    addr_out(i);
    ADDRCONT &= ~CAS;
    ADDRCONT |= CAS;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi there.");

  DDRC |= 0x300xff;
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
  write(0,val);
  Serial.print(val, HEX);
  refresh();
  Serial.println();
}
