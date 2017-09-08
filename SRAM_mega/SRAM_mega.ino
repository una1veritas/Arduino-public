#include "sram.h"

/* sram_avr.c functions */

void sram_init() {
  ADDRL_DIR = 0xff;
  ADDRH_DIR = 0xff;
  ADDRX_DIR |= ADDRX_MASK;
  DATA_DIR = 0x00;
  CONTROL_DIR |= ( SRAM_CS | SRAM_WE | SRAM_OE ); //| LATCH_L );
  CONTROL |= ( SRAM_CS | SRAM_WE | SRAM_OE ); //| LATCH_L );
}

inline void addr_out(uint32_t addr) {
  ADDRL = addr & 0xff;
  addr >>= 8;
  ADDRH = addr & 0xff;
  addr >>= 8;
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= addr & ADDRX_MASK;
}


unsigned char sram_read(unsigned short addr) {
  unsigned char val;
  CONTROL &= ~SRAM_CS;
  addr_out(addr);  // A16 is always 0
  DATA_DIR = 0x00;
  CONTROL &= ~SRAM_OE;
  CONTROL |= ~SRAM_OE;
  val = DATA_IN;
  CONTROL |= SRAM_CS;
  return val;
}

void sram_write(unsigned short addr, unsigned char data) {
  CONTROL &= ~SRAM_CS;
  addr_out(addr);  // A16 is always 0
  DATA_DIR = 0xff;
  DATA_OUT = data;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;
  CONTROL |= SRAM_CS;
}
/* sram_avr.c end */

const char text[] = "If I speak in the tongues of men or of angels, but do not have love, I am only a resounding gong or a clanging cymbal. If I have the gift of prophecy and can fathom all mysteries and all knowledge, and if I have a faith that can move mountains, but do not have love, I am nothing. If I give all I possess to the poor and give over my body to hardship that I may boast, but do not have love, I gain nothing.\n\nLove is patient, love is kind. It does not envy, it does not boast, it is not proud. It does not dishonor others, it is not self-seeking, it is not easily angered, it keeps no record of wrongs. Love does not delight in evil but rejoices with the truth. It always protects, always trusts, always hopes, always perseveres.\n\nLove never fails. But where there are prophecies, they will cease; where there are tongues, they will be stilled; where there is knowledge, it will pass away. For we know in part and we prophesy in part, but when completeness comes, what is in part disappears. When I was a child, I talked like a child, I thought like a child, I reasoned like a child. When I became a man, I put the ways of childhood behind me. For now we see only a reflection as in a mirror; then we shall see face to face. Now I know in part; then I shall know fully, even as I am fully known.\n\nAnd now these three remain: faith, hope and love. But the greatest of these is love.\n";
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
  uint8_t val;
  addr = random(0, 0x1ffff);
  
  Serial.println(addr, HEX);
  for(uint16_t i = 0; i < 16; i++) {
    val = random(0, 256);
    Serial.print(val, HEX);
    Serial.print(' ');
    sram_write(addr+i, val);
    CONTROL |= SRAM_CS;
  }
  Serial.println();
  delay(500);

  for(uint16_t i = 0; i < 16; i++) {
    val = sram_read(addr+i);
    Serial.print(val, HEX);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(500);
}

