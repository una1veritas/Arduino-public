
#define RAS_PORT PORTL
#define CAS_PORT PORTL
#define OE_PORT PORTL
#define WE_PORT PORTL
#define RAS_DDR DDRL
#define CAS_DDR DDRL
#define OE_DDR DDRL
#define WE_DDR DDRL

#define DRAM_CONTROL PORTL
#define DRAM_CONTROL_DDR DDRL

#define ADDRL_PORT PORTA
#define ADDRH_PORT PORTC
#define ADDRL_DDR DDRA
#define ADDRH_DDR DDRC
#define ADDRL_MASK 0xff
#define ADDRH_MASK 0x03

#define RAS_BIT (1<<0)
#define CAS_BIT (1<<1)
#define OE_BIT (1<<2)
#define WE_BIT (1<<3)

#define DATA_OUT  PORTF
#define DATA_IN   PINF
#define DATA_DDR  DDRF
#define DATA_MASK 0x0f

uint8_t crc8(uint8_t inbyte, uint8_t & crc) {
    uint8_t j;
    uint8_t mix;

    for (j = 0; j < 8; j++) {
        mix = (crc ^ inbyte) & 0x01;
        crc >>= 1;
        if (mix) crc ^= 0x8C;
        inbyte >>= 1;
    }
    return crc;
}


void addr_out(uint16_t addr) {
  ADDRL_PORT = addr & ADDRL_MASK;
  addr >>= 8;
  ADDRH_PORT = addr & ADDRH_MASK;
}

void data_out(uint8_t val) {
  DATA_DDR |= DATA_MASK; // No mode default
  DATA_OUT = val;
  WE_PORT &= ~WE_BIT; //digitalWrite(DRAM_WE_PIN,LOW);
  WE_PORT |= WE_BIT; //digitalWrite(DRAM_WE_PIN,HIGH);
}

uint8_t data_in() {
  uint8_t val;
  DATA_DDR &= ~DATA_MASK;
  OE_PORT &= ~OE_BIT; //digitalWrite(DRAM_OE_PIN,LOW);
  /*
  __asm__ __volatile("nop");
  val = DATA_IN;
  */
  OE_PORT |= OE_BIT; //digitalWrite(DRAM_OE_PIN,HIGH);
  val = DATA_IN;
  return val & DATA_MASK;
}

uint8_t dram_read(const uint32_t & addr) {
  uint8_t data;
  uint16_t row, col;

  // low 4 bit in even address, high 4 bit in the next odd address
  row = addr>>9;
  col = addr<<1;

  cli();
  addr_out(row);
  RAS_PORT &= ~RAS_BIT; //digitalWrite(DRAM_RAS_PIN,LOW);
  
  // low nibble
  addr_out(col);
  CAS_PORT &= ~CAS_BIT; //digitalWrite(DRAM_CAS_PIN,LOW);
  //OE_PORT &= ~OE_BIT; //digitalWrite(DRAM_OE_PIN,LOW);
  data = data_in();
  //OE_PORT |= OE_BIT; //digitalWrite(DRAM_OE_PIN,HIGH);
  CAS_PORT |= CAS_BIT; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // high nibble
  addr_out(col | 1);
  CAS_PORT &= ~CAS_BIT; //digitalWrite(DRAM_CAS_PIN,LOW);
  //OE_PORT &= ~OE_BIT; //digitalWrite(DRAM_OE_PIN,LOW);
  data |= data_in()<<4;
  //OE_PORT |= OE_BIT; //digitalWrite(DRAM_OE_PIN,HIGH);
  CAS_PORT |= CAS_BIT; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // 8 bits read completed
  RAS_PORT |= RAS_BIT; //ådigitalWrite(DRAM_RAS_PIN,HIGH);
  
  sei();
  return data;  
}

void dram_write(const uint32_t addr, uint8_t data) {
  uint16_t row, col;

  row = addr>>9;
  col = addr<<1;

  cli();
  addr_out(row);
  RAS_PORT &= ~RAS_BIT; //digitalWrite(DRAM_RAS_PIN,LOW);
  // low nibble
  addr_out(col);
  CAS_PORT &= ~CAS_BIT; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data);
  CAS_PORT |= CAS_BIT; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // high nibble
  addr_out(col | 1);
  CAS_PORT &= ~CAS_BIT; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data>>4);
  CAS_PORT |= CAS_BIT; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // 8 bits read completed

  RAS_PORT |= RAS_BIT; //digitalWrite(DRAM_RAS_PIN,HIGH);
  sei();
}

void init_timer1_interrupt() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  
  OCR1A = 1562; // RAS_only: 384; // CAS_before_RAS: 1562; //30;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);

  CAS_before_RAS();
  sei();      // interrupts are also enabled here, starting refresh 
}

volatile uint16_t rowcount;

inline void RAS_only(void) {
  // refreshes a quadrant (256 rows) at a time
  for(uint16_t i = 0; i < 0x100; i++, rowcount++) {
    addr_out(rowcount);
    RAS_PORT &= ~RAS_BIT;
    RAS_PORT |= RAS_BIT;
  }
  rowcount &= 0x300; // starts from rows 0x000, 0x100, 0x200, 0x300
}

inline void CAS_before_RAS(void) {
  for(rowcount = 0; rowcount < 0x400; rowcount++ ) {
      CAS_PORT &= ~CAS_BIT;
      RAS_PORT &= ~RAS_BIT;
      CAS_PORT |= CAS_BIT;
      RAS_PORT |= RAS_BIT;
  }  
}

ISR(TIMER1_COMPA_vect)
{
  //RAS_only();
  CAS_before_RAS();
}

long errcount = 0;

void test() {
  uint32_t counter;
  for(;;) {
    data_out(counter & DATA_MASK);
    delay(16);
    counter++;
  }
}

void setup() {
  Serial.begin(19200);
  Serial.println("Hi there.");


  RAS_DDR |= RAS_BIT;
  CAS_DDR |= CAS_BIT;
  OE_DDR |= OE_BIT;
  WE_DDR |= WE_BIT;
  RAS_PORT |= RAS_BIT;
  CAS_PORT |= CAS_BIT;
  OE_PORT |= OE_BIT;
  WE_PORT |= WE_BIT;

  ADDRL_DDR |= 0xff;
  ADDRH_DDR |= ADDRH_MASK;

  DATA_DDR |= DATA_MASK;

  init_timer1_interrupt();
  Serial.println("Let's go!");
  //test();
}

void loop() {
  uint32_t addr;
  uint8_t data, crc8val;
  
  addr = random(0,0x80000-1);

  Serial.print("from 0x");
  Serial.println(addr, HEX);
  Serial.print("write ");

  crc8val = 0;
  for(uint32_t i = 0; i < 128; i++) {
    data = random('0','Z');
    Serial.print((char)data);
    
    dram_write(addr+i, data);
    crc8(data, crc8val);
  }
  dram_write(addr+64, crc8val);
  Serial.println();
  delay(200);

  // read
  Serial.print("read  ");
  crc8val = 0;
  for(uint32_t i = 0; i < 128; i++) {
    data = dram_read(addr+i);
    crc8(data, crc8val);

    Serial.print((char)data);
  }
  data = dram_read(addr+64);
  crc8(data,crc8val);
  Serial.print(": ");
  Serial.print(crc8val, HEX);
  if ( crc8val ) {
    errcount++;
    Serial.print("Error = ");
    Serial.print(errcount);
  }
  Serial.println();

  Serial.println();
  //
//  addr++;
//  addr &= 0x000f;
  delay(600);
}

