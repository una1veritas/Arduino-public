
const byte DRAM_RAS = 1<<2;  // PB2
const byte DRAM_RAS_PIN = 10;
const byte DRAM_CAS = 1<<3;  // PB3
const byte DRAM_CAS_PIN = 11;
const byte DRAM_OE = 1<<4;   // PB4
const byte DRAM_OE_PIN = 12;
const byte DRAM_WE = 1<<5;   // PB5
const byte DRAM_WE_PIN = 13;

const byte ADDR_BUS[] = {
  14, 15, 16, 17, 18, 19, 6, 7, 
  8, 9,
};
const byte ADDR_BUS_WIDTH = 10;
const byte DATA_IO[] = {
  2, 3, 4, 5,
};
const byte DATA_IO_WIDTH = 4;


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

uint32_t addr = 0;
void setup() {
  Serial.begin(19200);
  Serial.println("Hi there.");

  pinMode(DRAM_RAS_PIN, OUTPUT);
  digitalWrite(DRAM_RAS_PIN, HIGH);
  pinMode(DRAM_CAS_PIN, OUTPUT);
  digitalWrite(DRAM_CAS_PIN, HIGH);
  pinMode(DRAM_OE_PIN, OUTPUT);
  digitalWrite(DRAM_OE_PIN, HIGH);
  pinMode(DRAM_WE_PIN, OUTPUT);
  digitalWrite(DRAM_WE_PIN, HIGH);

  for(int i = 0; i < ADDR_BUS_WIDTH; i++) {
    pinMode(ADDR_BUS[i], OUTPUT);
  }
  for(int i = 0; i < DATA_IO_WIDTH; i++) {
    pinMode(DATA_IO[i], OUTPUT);
  }

  init_timer1_interrupt();
  Serial.println("Let's go!");
}

void loop() {
  uint8_t data;  

  addr = random(0,0x40000-1);

  Serial.print("from 0x");
  Serial.println(addr, HEX);
  Serial.print("write ");

  for(uint32_t i = 0; i < 64; i++) {
    data = random('0','Z');
    Serial.print((char)data);
  /*
 // write
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
  __asm__ __volatile__("nop");
  addr_out(col | 1);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data>>4);
  PORTB &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  PORTB |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // 8 bits read completed
  PORTB |= DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,HIGH);
  sei();
*/
  dram_write(addr+i, data);
  }
  Serial.println();

  // read
  Serial.print("read  ");
  for(uint32_t i = 0; i < 64; i++) {
   data = dram_read(addr+i);
  /*
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
  __asm__ __volatile__("nop");
  addr_out(col | 1);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  PORTB &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data |= data_in()<<4;
  PORTB |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  // 8 bits read completed
  PORTB |= DRAM_RAS; //ådigitalWrite(DRAM_RAS_PIN,HIGH);

  sei();
*/
  Serial.print((char)data);
  }
  Serial.println();
  Serial.println();
  //
//  addr++;
//  addr &= 0x000f;
  delay(300);
}

