
#define DRAM_RAS_PORT PORTB
#define DRAM_CAS_PORT PORTC
#define DRAM_OE_PORT PORTD
#define DRAM_WE_PORT PORTC
#define DRAM_RAS_DDR DDRB
#define DRAM_CAS_DDR DDRC
#define DRAM_OE_DDR DDRD
#define DRAM_WE_DDR DDRC

#define DRAM_RAS (1<<PB5)  
#define DRAM_CAS (1<<PC5) 
#define DRAM_OE (1<<PD2)
#define DRAM_WE (1<<PC1)

const byte ADDR_BUS[] = {
  12, 11, 10, 9, 8, 
  5, 6, 7, 
  3, 
};
const byte ADDR_BUS_WIDTH = 9;

const byte DATA_IO[] = {
  14, 16, 18, 17,
};
const byte DATA_IO_WIDTH = 4;

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
  for(uint8_t bit = 0; bit < ADDR_BUS_WIDTH; bit++) {
    digitalWrite(ADDR_BUS[bit], addr>>bit & 1);
  }
}

void data_out(uint8_t val) {
  for(uint8_t bit = 0; bit < DATA_IO_WIDTH; bit++) {
    pinMode(DATA_IO[bit], OUTPUT);
    digitalWrite(DATA_IO[bit], val>>bit & 1);
  }
}

uint8_t data_in() {
  uint8_t val = 0;
  for(uint8_t bit = 0; bit < DATA_IO_WIDTH; bit++) {
    pinMode(DATA_IO[bit], INPUT);
    if ( digitalRead(DATA_IO[bit]) )
      val |= (1<<bit);
  }
  return val;
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
  DRAM_CAS_PORT &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  DRAM_OE_PORT &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data = data_in();
  DRAM_OE_PORT |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  DRAM_CAS_PORT |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // high nibble
  addr_out(col | 1);
  DRAM_CAS_PORT &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  DRAM_OE_PORT &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data |= data_in()<<4;
  DRAM_OE_PORT |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  DRAM_CAS_PORT |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);

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
  DRAM_RAS_PORT &= ~DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,LOW);
  // low nibble
  addr_out(col);
  DRAM_CAS_PORT &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data);
  DRAM_WE_PORT &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  DRAM_WE_PORT |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
  DRAM_CAS_PORT |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // high nibble
  addr_out(col | 1);
  DRAM_CAS_PORT &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data>>4);
  DRAM_WE_PORT &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  DRAM_WE_PORT |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
  DRAM_CAS_PORT |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // 8 bits read completed
  DRAM_RAS_PORT |= DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,HIGH);
  sei();
}
void init_timer1_interrupt() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  
  OCR1A = 625; // CAS_before_RAS: 1562; //30;
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
  // refreshes 256 rows at a time
  for(uint16_t i = 0; i < 0x80; i++, rowcount++) {
    addr_out(rowcount);
    DRAM_RAS_PORT &= ~DRAM_RAS;
    DRAM_RAS_PORT |= DRAM_RAS;
  }
  rowcount &= 0x1ff; // starts from rows 0x000, 0x100, 0x200, 0x300
}

inline void CAS_before_RAS(void) {
  for(rowcount = 0; rowcount < 0x400; rowcount++ ) {
      DRAM_CAS_PORT &= ~DRAM_CAS;
      DRAM_RAS_PORT &= ~DRAM_RAS;
      DRAM_CAS_PORT |= DRAM_CAS;
      DRAM_RAS_PORT |= DRAM_RAS;
  }  
}

ISR(TIMER1_COMPA_vect)
{
  RAS_only();
  //CAS_before_RAS();
}

const uint16_t blocksize = 128;
long errcount = 0;
uint32_t addr = 0;
uint8_t buf[blocksize];

void setup() {
  Serial.begin(38400);
  Serial.println("Hi there.");

  // let sdcard stay inactive by CS high
  DDRD |= (1<<PD4);
  PORTD |= (1<<PD4);
  //
  
  DRAM_RAS_DDR |= DRAM_RAS;
  DRAM_CAS_DDR |= DRAM_CAS;
  DRAM_OE_DDR |= DRAM_OE;
  DRAM_WE_DDR |= DRAM_WE;
  DRAM_RAS_PORT |= DRAM_RAS;
  DRAM_CAS_PORT |= DRAM_CAS;
  DRAM_OE_PORT |= DRAM_OE;
  DRAM_WE_PORT |= DRAM_WE;

  for(uint8_t bit = 0; bit < ADDR_BUS_WIDTH; bit++) {
    pinMode(ADDR_BUS[bit], OUTPUT);
  }
  init_timer1_interrupt();
  Serial.println("Let's go!");
}

void loop() {
  uint8_t data, crc8val;
  
  Serial.print("0x");
  Serial.print(addr, HEX);
  Serial.print(" write: ");

  randomSeed(addr);
  
  crc8val = 0;
  for(uint32_t i = 0; i < blocksize; i++) {
    data = random(0, 256);
    buf[i] = data;
    if ( (i & 0x1f) == 0 )
      Serial.println();
    Serial.print(data>>4&0x0f, HEX);
    Serial.print(data&0x0f, HEX);
    Serial.print(' ');
    
    dram_write(addr+i, data);
    crc8(data, crc8val);
  }
  dram_write(addr+blocksize, crc8val);
  Serial.println();
  //delay(200);

  // read
  Serial.print("read  ");
  crc8val = 0;
  for(uint32_t i = 0; i < blocksize; i++) {
    data = dram_read(addr+i);
    if ( data != buf[i])
      errcount++;
    crc8(data, crc8val);
  }
  data = dram_read(addr+blocksize);
  crc8(data,crc8val);
  Serial.print(": ");
  if ( crc8val == 0 ) {
    Serial.println("ok");
  } else {
    Serial.print("\nError! ");
    Serial.print(errcount);
    //errcount = 0;
    delay(10000);
  }
  Serial.println();
  
  addr += blocksize;
  if ( addr > 0x1ffff ) {
    Serial.print("total error count ");
    Serial.println(errcount);
    while (1);
  }
  //delay(100);
}

