
#define ADDRL        PORTA
#define ADDRL_DIR    DDRA
#define ADDRH        PORTC
#define ADDRH_DIR    DDRC
#define ADDRX        PORTB
#define ADDRX_DIR    DDRB
const uint8_t ADDRX_BIT = 1<<0; // PB0

#define DATA         PORTC
#define DATA_DIR     DDRC

#define CONTROL       PORTB
#define CONTROL_DIR   DDRB

const uint8_t SRAM_CS = 1<<3; // PB3; // and LATCH_OE
const uint8_t SRAM_OE = 1<<2; // PB3;
const uint8_t SRAM_WE = 1<<1; // PB3;
const uint8_t ADDRL_LATCH = 1<<7; // PB7;


long count = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi, me sanguino.");

  ADDRL_DIR = 0xff;
  ADDRH_DIR = 0xff;
  ADDRX_DIR |= ADDRX_BIT;
  CONTROL_DIR = ( SRAM_CS | SRAM_OE | SRAM_WE | ADDRL_LATCH );
  
  CONTROL |= ( SRAM_CS | SRAM_OE | SRAM_WE | SRAM_ADDRL_LATCH );
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(count & 0xff, BIN);

  CONTROL &= ~SRAM_CS;
  delay(500);
  ADDRL_DIR = 0xff;
  ADDRL = count & 0xff;
  ADDRH = count>>8 & 0xff;
  ADDRX |= ( count>>16 & ADDRX_BIT);
  CONTROL &= ~ADDRL_LATCH;
  CONTROL |= ADDRL_LATCH;
  DATA = count & 0xff;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;

  CONTROL |= SRAM_CS;
  delay(500);
  count++;
}
