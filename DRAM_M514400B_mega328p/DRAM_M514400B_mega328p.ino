
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

void init_timer1_interrupt() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  
  OCR1A = 1562;//30;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  sei();      // interrupts are also enabled here, starting refresh 
}

volatile uint32_t rowcount;

ISR(TIMER1_COMPA_vect)
{
  for(rowcount = 0; rowcount < 0x400; rowcount++ ) {
      PORTB &= ~DRAM_CAS;
      PORTB &= ~DRAM_RAS;
      PORTB |= DRAM_CAS;
      PORTB |= DRAM_RAS;
  }
  /*
  // Refresh DRAM cells every 2mS
  int i;
  for (i=0;i<128;i++)  // 128 cycles
  {
    //digitalWrite(13, HIGH); // RAS high
    //digitalWrite(12, HIGH); // CAS high
    PORTB |= B010000;   // CAS high
    PORTB |= B100000;   // RAS high
  
    //digitalWrite(8, HIGH && (i & B01000000));
    PORTB |= (i & B01000000) >> 6;
    //digitalWrite(7, HIGH && (i & B00100000));
    PORTD |= (i & B00100000) << 2; 
    //digitalWrite(6, HIGH && (i & B00010000));
    PORTD |= (i & B00010000) << 2;
    //digitalWrite(5, HIGH && (i & B00001000));
    PORTD |= (i & B00001000) << 2; 
    //digitalWrite(4, HIGH && (i & B00000100));
    PORTD |= (i & B00000100) << 2; 
    //digitalWrite(3, HIGH && (i & B00000010));
    PORTD |= (i & B00000010) << 2; 
    //digitalWrite(2, HIGH && (i & B00000001));
    PORTD |= (i & B00000001) << 2; 
    //digitalWrite(13, LOW);  // RAS low
    //digitalWrite(13, HIGH); // RAS high
    PORTB ^= B011111;   // RAS low
    PORTB |= B100000;   // RAS high
  }
  */
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
  delay(200);  // wait one or more refresh
  Serial.println("Let's go!");
}

void loop() {
  uint16_t row;
  uint8_t val = random('0','Z');  
  uint8_t data = 0;

  addr = millis()>>10;
  data = val & 0x0f;

  Serial.print("write ");
  Serial.print(addr, HEX);
  Serial.print(" ");
  Serial.print(data, HEX);
  Serial.print(", ");

 // write
  row = addr>>10;
  cli();
  addr_out(row);
  PORTB &= ~DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,LOW);
  addr_out(addr);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  data_out(data);
  PORTB &= ~DRAM_WE; //digitalWrite(DRAM_WE_PIN,LOW);
  PORTB |= DRAM_WE; //digitalWrite(DRAM_WE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  PORTB |= DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,HIGH);
  sei();

  delay(20);
  data_out(0xff);
  // read
  row = addr>>10;

  cli();
  addr_out(row);
  PORTB &= ~DRAM_RAS; //digitalWrite(DRAM_RAS_PIN,LOW);
  addr_out(addr);
  PORTB &= ~DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,LOW);
  PORTB &= ~DRAM_OE; //digitalWrite(DRAM_OE_PIN,LOW);
  data = data_in();
  PORTB |= DRAM_OE; //digitalWrite(DRAM_OE_PIN,HIGH);
  PORTB |= DRAM_CAS; //digitalWrite(DRAM_CAS_PIN,HIGH);
  PORTB |= DRAM_RAS; //ådigitalWrite(DRAM_RAS_PIN,HIGH);
  sei();

  Serial.print("read ");
  Serial.print(data, HEX);
  Serial.println(".");
  //
//  addr++;
//  addr &= 0x000f;
  delay(300);
}

