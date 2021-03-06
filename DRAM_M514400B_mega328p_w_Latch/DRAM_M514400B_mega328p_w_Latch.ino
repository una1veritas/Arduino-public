

#define DRAM_RAS_PORT PORTB
#define DRAM_CAS_PORT PORTB
#define DRAM_OE_PORT PORTB
#define DRAM_WE_PORT PORTB
#define DRAM_RAS_DDR DDRB
#define DRAM_CAS_DDR DDRB
#define DRAM_OE_DDR DDRB
#define DRAM_WE_DDR DDRB

const byte DRAM_WE = 1<<5;   // PB5
const byte DRAM_OE = 1<<4;   // PB4
const byte DRAM_CAS = 1<<3;  // PB3
const byte DRAM_RAS = 1<<2;  // PB2

#define DATA_OUT  PORTC
#define DATA_IN   PINC
#define DATA_DDR  DDRC
#define DATA_MASK 0x0f

// PC0 -- PC5
#define ADDRL_PORT  PORTC
#define ADDRL_DDR   DDRC
#define ADDRL_MASK  0x3f
// PD6, PD7
#define ADDRM_PORT  PORTD
#define ADDRM_DDR   DDRD
#define ADDRM_MASK  0xc0
// PD4, PD5
#define ADDRH_PORT  PORTD
#define ADDRH_DDR   DDRD
#define ADDRH_MASK  0x30

#define ADDR_LATCH_PORT  PORTB
#define ADDR_LATCH_DDR   DDRB
#define ADDR_LATCH       1<<0

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


inline void addr_out(uint16_t addr) {
  // ADDRL bits are output in default
  ADDRL_DDR  |= ADDRL_MASK;
  ADDRL_PORT = (uint8_t) addr; // & ADDRL_MASK;
  /*
  ADDRM_PORT &= ~ADDRM_MASK;
  ADDRH_PORT &= ~ADDRH_MASK;
  ADDRM_PORT |= (uint8_t) addr & ADDRM_MASK;
  ADDRH_PORT |= (uint8_t)(addr>>4) & ADDRH_MASK;
  */
  ADDRM_PORT &= ~(ADDRM_MASK | ADDRH_MASK);
  ADDRM_PORT |= (uint8_t) addr & ADDRM_MASK;
  ADDRH_PORT |= (uint8_t)(addr>>4) & ADDRH_MASK;
  ADDR_LATCH_PORT &= ~ADDR_LATCH;
  ADDR_LATCH_PORT |= ADDR_LATCH;
}

inline void data_out(uint8_t val) {
  // assuming no default pin mode
  DATA_DDR |= DATA_MASK;  
  DATA_OUT = val;
  DRAM_WE_PORT &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  DRAM_WE_PORT |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
}

inline uint8_t data_in() {
  uint8_t val;
  DATA_DDR &= ~DATA_MASK;
  DRAM_OE_PORT &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  /*
  __asm__ __volatile("nop");
  val = DATA_IN;
  */
  DRAM_OE_PORT |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
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
  DRAM_RAS_PORT &= ~DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,LOW);

  // low nibble
  addr_out(col);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  //PORTB &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data = data_in();
  //PORTB |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // high nibble
  addr_out(col | 1);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  //PORTB &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data |= (data_in()<<4);
  //PORTB |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // 8 bits read completed
  DRAM_RAS_PORT |= DRAM_RAS; //ådigitalWrite(DRAM_RAS_PIN,HIGH);
  
  sei();
  return data;  
}

void dram_write(const uint32_t addr, uint8_t data) {
  uint16_t row, col;

  row = addr>>9;
  col = addr<<1;

  cli();
  addr_out(row);
  PORTB &= ~DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,LOW);
  // low nibble
  addr_out(col);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // high nibble
  addr_out(col | 1);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data>>4);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // 8 bits read completed
  PORTB |= DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,HIGH);
  sei();
}
void init_timer1_interrupt() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  
  OCR1A = 384; // CAS_before_RAS: 1562; //30;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  sei();      // interrupts are also enabled here, starting refresh 
}

volatile uint16_t rowcount;

inline void RAS_only(void) {
  // refreshes a quadrant (256 rows) at a time
  for(uint16_t i = 0; i < 0x100; i++, rowcount++) {
    addr_out(rowcount);
    PORTB &= ~DRAM_RAS;
    PORTB |= DRAM_RAS;
  }
  rowcount &= 0x300; // starts from rows 0x000, 0x100, 0x200, 0x300
}

inline void CAS_before_RAS(void) {
  for(rowcount = 0; rowcount < 0x400; rowcount++ ) {
      PORTB &= ~DRAM_CAS;
      PORTB &= ~DRAM_RAS;
      PORTB |= DRAM_CAS;
      PORTB |= DRAM_RAS;
  }  
}

ISR(TIMER1_COMPA_vect)
{
  //RAS_only();
  CAS_before_RAS();
}

long errcount = 0;

void test() {
  Serial.println("testing..");
  for(;;) {
    for(uint32_t addr = 0; addr < 0x40000; addr++) {
      addr_out(addr);
      delay(8);
    }
  }
}

void init_dram() {
  DRAM_RAS_DDR |= DRAM_RAS;
  DRAM_CAS_DDR |= DRAM_CAS;
  DRAM_OE_DDR |= DRAM_OE;
  DRAM_WE_DDR |= DRAM_WE;
  DRAM_RAS_PORT |= DRAM_RAS;
  DRAM_CAS_PORT |= DRAM_CAS;
  DRAM_OE_PORT |= DRAM_OE;
  DRAM_WE_PORT |= DRAM_WE;
  ADDR_LATCH_DDR |= ADDR_LATCH;
  ADDR_LATCH_PORT |= ADDR_LATCH;

  ADDRL_DDR |= ADDRL_MASK;
  ADDRM_DDR |= ADDRM_MASK;
  ADDRH_DDR |= ADDRH_MASK;

  DATA_DDR |= DATA_MASK;

  CAS_before_RAS();  
}

void setup() {
  Serial.begin(19200);
  Serial.println("Hi there.");

  init_dram();
  init_timer1_interrupt();
  Serial.println("Let's go!");

}

void loop() {
  uint32_t addr;
  uint8_t data, crc8val;
  const uint16_t testBytes = 512;
  long swatch;
  
  addr = random(0,0x80000);
  randomSeed(addr);

  Serial.print("From 0x");
  Serial.print(addr, HEX);
  Serial.print(", ");

  Serial.print(testBytes, DEC);
  Serial.print(" bytes write ");
  crc8val = 0;
  swatch = micros();
  for(uint32_t i = 0; i < testBytes; i++) {
    data = random(0,256);
    
    dram_write(addr+i, data);
    crc8(data, crc8val);
  }
  dram_write(addr+testBytes, crc8val);
  swatch = micros() - swatch;
  Serial.print("(");
  Serial.print(swatch, DEC);
  Serial.print("), ");
  //delay(200);

  // read
  Serial.print("read ");
  crc8val = 0;
  swatch = micros();
  for(uint32_t i = 0; i < testBytes; i++) {
    data = dram_read(addr+i);
    crc8(data, crc8val);
  }
  data = dram_read(addr+testBytes);
  swatch = micros() - swatch;
  Serial.print("(");
  Serial.print(swatch, DEC);
  Serial.print("), ");
  crc8(data,crc8val);
  Serial.print(crc8val, HEX);
  if ( crc8val ) {
    errcount++;
    Serial.print("Error! ");
    Serial.print(errcount);
    Serial.println();
    randomSeed(addr);
    for(uint32_t i = 0; i < testBytes; i++) {
      data = random(0, 256);
      Serial.print("0x");
      Serial.print(data >> 4 & 0x0f, HEX);
      Serial.print(data & 0x0f, HEX);
      Serial.print(" ");
    }
    Serial.println();
    crc8val = 0;
    for(uint32_t i = 0; i < testBytes; i++) {
      data = dram_read(addr+i);
      Serial.print("0x");
      Serial.print(data >> 4 & 0x0f, HEX);
      Serial.print(data & 0x0f, HEX);
      Serial.print(" ");
      crc8(data, crc8val);
    }
    data = dram_read(addr+testBytes);
    crc8(data,crc8val);
    Serial.print(": ");
    Serial.print(crc8val, HEX);
    Serial.println();
  }
  Serial.println();

  Serial.println();
  //
//  addr++;
//  addr &= 0x000f;
  delay(600);
}

