/* SRAM I/F Port/Pin definitions */
#define ADDRL_DIR DDRA
#define ADDRL PORTA
#define ADDRH_DIR DDRC
#define ADDRH PORTC
#define ADDRX_DIR DDRL
#define ADDRX PORTL
#define ADDRX_MASK (1<<0)

#define DATA_OUT  PORTF
#define DATA_IN  PINF
#define DATA_DIR DDRF

#define CONTROL PORTL
#define CONTROL_DIR DDRL
#define SRAM_CS (1<<1)
#define SRAM_OE (1<<2)
#define SRAM_WE (1<<3)
//#define LATCH_L (1<<7)

long count = 0;
void setup() {
  // put your setup code here, to run once:

  CONTROL_DIR = 0xff;
  CONTROL = 0xff;
  ADDRL_DIR = 0xff;
  ADDRH_DIR = 0xff;
  ADDRX_DIR = ADDRX_MASK;
  ADDRL = 0;
  ADDRH = 0;
  ADDRX &= ~ADDRX_MASK;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1);
  ADDRL = count & 0xff;
  ADDRH = count>>8 & 0xff;
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= count >> 16 & ADDRX_MASK;
  count++;
}
