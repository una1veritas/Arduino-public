
#define ADDRL PORTA
#define ADDRL_DIR DDRA
#define ADDRH PORTC
#define ADDRH_DIR DDRC
#define ADDRX_DIR DDRB
#define ADDRX PORTB
#define ADDRX_MASK (1<<0)

#define DATA_OUT  PORTA
#define DATA_IN  PINA
#define DATA_DIR DDRA

#define CONTROL PORTB
#define CONTROL_DIR DDRB
#define SRAM_CS (1<<3)
#define SRAM_OE (1<<2)
#define SRAM_WE (1<<1)
#define LATCH_L (1<<7)

void addr_out(uint32_t addr) {
  ADDRL = addr & 0xff;
  ADDRL_DIR = 0xff;
  CONTROL &= ~LATCH_L;
  delay(20);
  CONTROL |= LATCH_L;
  addr >>= 8;
  ADDRH = addr & 0xff;
  addr >>= 8;
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= addr & ADDRX_MASK;
}

void data_out(uint8_t val) {
  DATA_OUT = val;
}

uint8_t data_in() {
  uint8_t val;
  DATA_DIR = 0xff;
  CONTROL &= ~SRAM_OE;
  __asm__ __volatile("nop");
  val = DATA_IN;
  delay(0);
  CONTROL |= ~SRAM_OE;
  return val;
}

void sram_init() {
  ADDRL_DIR = 0xff;
  ADDRH_DIR = 0xff;
  ADDRX_DIR |= ADDRX_MASK;
  CONTROL_DIR |= ( SRAM_CS | SRAM_WE | SRAM_OE | LATCH_L );

  CONTROL |= ( SRAM_CS | SRAM_WE | SRAM_OE | LATCH_L );
}

long addr = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi, there.");
  Serial.println("Let's start.");

  sram_init();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t val = random('0','z');
  Serial.println(addr, HEX);
  Serial.println((char)val);
  CONTROL &= ~SRAM_CS;

  addr_out(addr);
  data_out(val);
  CONTROL &= ~SRAM_WE;
  delay(20);
  CONTROL |= SRAM_WE;
  
  CONTROL |= SRAM_CS;
  delay(500);

  CONTROL &= ~SRAM_CS;
  delay(500);
  addr_out(addr);
  val = data_in();
  
  CONTROL |= SRAM_CS;
  Serial.println((char)val);
  delay(500);
  Serial.println();
  addr++;
}
