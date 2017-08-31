
#define DRAM_RAS_PORT PORTB
#define DRAM_CAS_PORT PORTB
#define DRAM_OE_PORT PORTB
#define DRAM_WE_PORT PORTB
#define DRAM_RAS_DDR DDRB
#define DRAM_CAS_DDR DDRB
#define DRAM_OE_DDR DDRB
#define DRAM_WE_DDR DDRB

const byte DRAM_RAS = 1<<2;  // PB2
const byte DRAM_CAS = 1<<3;  // PB3
const byte DRAM_OE = 1<<4;   // PB4
const byte DRAM_WE = 1<<5;   // PB5
/*
const byte ADDR_BUS[] = {
  14, 15, 16, 17, 18, 19, 6, 7, 
  8, 9,
};
const byte ADDR_BUS_WIDTH = 10;
const byte DATA_IO[] = {
  2, 3, 4, 5,
};
const byte DATA_IO_WIDTH = 4;
*/
#define DATA_PORT PORTD
#define DATA_DDR  DDRD

#define ADDRL_PORT PORTC
#define ADDRM_PORT PORTD
#define ADDRH_PORT PORTB
#define ADDRL_DDR DDRC
#define ADDRM_DDR DDRD
#define ADDRH_DDR DDRB

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
  PORTC = (addr & 0x3f);
  PORTD &= ~0xc0;
  PORTD |= (0xc0 & addr);
  addr >>= 8;
  PORTB &= ~0x03;
  PORTB |= (0x03 & addr);
}

void data_out(uint8_t val) {
  DDRD |= (0x0f << 2);  
  PORTD &= ~(0x0f << 2);
  PORTD |= (val & 0x0f)<<2;
}

uint8_t data_in() {
  uint8_t val;
  DDRD &= ~(0x0f << 2);    
  __asm__ __volatile("nop");
  val = PIND & (0x0f<<2);
  val >>= 2;
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
  PORTB &= ~DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,LOW);

  // low nibble
  addr_out(col);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  PORTB &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data = data_in();
  PORTB |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // high nibble
  addr_out(col | 1);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  PORTB &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data |= data_in()<<4;
  PORTB |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);

  // 8 bits read completed
  PORTB |= DRAM_RAS; //ådigitalWrite(DRAM_RAS_PIN,HIGH);
  
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
  PORTB &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  PORTB |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // high nibble
  addr_out(col | 1);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data>>4);
  PORTB &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  PORTB |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
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

  CAS_before_RAS();
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
  RAS_only();
  //CAS_before_RAS();
}

long errcount = 0;

void setup() {
  Serial.begin(19200);
  Serial.println("Hi there.");


  DRAM_RAS_DDR |= DRAM_RAS;
  DRAM_CAS_DDR |= DRAM_CAS;
  DRAM_OE_DDR |= DRAM_OE;
  DRAM_WE_DDR |= DRAM_WE;
  DRAM_RAS_PORT |= DRAM_RAS;
  DRAM_CAS_PORT |= DRAM_CAS;
  DRAM_OE_PORT |= DRAM_OE;
  DRAM_WE_PORT |= DRAM_WE;

  ADDRL_DDR |= 0x3f;
  ADDRM_DDR |= 0xC0;
  ADDRH_DDR |= 0x03;

  DATA_DDR |= 0x0f<<2;
/*
  for(int i = 0; i < ADDR_BUS_WIDTH; i++) {
    pinMode(ADDR_BUS[i], OUTPUT);
  }
  for(int i = 0; i < DATA_IO_WIDTH; i++) {
    pinMode(DATA_IO[i], OUTPUT);
  }
  */

  init_timer1_interrupt();
  Serial.println("Let's go!");
}

void loop() {
  uint32_t addr;
  uint8_t data, crc8val;
  
  addr = random(0,0x40000-1);

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
    Serial.print("Error! ");
    Serial.print(errcount);
  }
  Serial.println();

  Serial.println();
  //
//  addr++;
//  addr &= 0x000f;
  delay(600);
}

