#include "sram.h"

/* sram_avr.c functions */
/* atmegaxxx0 XMEM configuration (HC374/574 latch) with A16 -- PD7 */

void sram_init() {
  ADDRL_DIR = ADDRL_MASK;
  ADDRH_DIR = ADDRH_MASK; // fixed to be OUTPUT
  ADDRX_DIR |= ADDRX_MASK; // fixed to be OUTPUT
  CONTROL_DIR |= ( SRAM_WE | SRAM_OE | SRAM_ALE );
  CONTROL |= ( SRAM_WE | SRAM_OE | SRAM_ALE);
  CONTROL_CS_DIR |= SRAM_CS;
  CONTROL_CS |= SRAM_CS;
}

inline void addr_out(uint16_t addr) {
  ADDRL_DIR = ADDRL_MASK;  // set to OUTPUT
  ADDRL = addr & ADDRL_MASK;
  CONTROL &= ~SRAM_ALE;  // Latch on this edge
  addr >>= 8;
  ADDRH = addr & ADDRH_MASK; // ADDRH is always OUTPUT
}

inline void sram_bank(uint8_t bk) {
  ADDRX &= ~ADDRX_MASK;
  if ( bk & 1 )
    ADDRX |= ADDRX_MASK;
}

uint8_t sram_read(uint16_t addr) {
  unsigned char val;
  CONTROL_CS &= ~SRAM_CS;
  addr_out(addr);
  DATA_OUT = 0x00; // clear the pull-ups
  DATA_DIR = 0x00;
  CONTROL &= ~SRAM_OE;
  CONTROL |= SRAM_OE; // valid data remains while
  val = DATA_IN;
  
  CONTROL |= SRAM_ALE;    // transparent
  CONTROL_CS |= SRAM_CS;
  return val;
}

void sram_write(uint16_t addr, uint8_t data) {
  CONTROL_CS &= ~SRAM_CS;
  addr_out(addr);  // A16 is always 0
  DATA_OUT = data;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;

  CONTROL |= SRAM_ALE;    // transparent
  CONTROL_CS |= SRAM_CS;
}

/* sram_avr.c end */

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

//const char text[] = "If I speak in the tongues of men or of angels, but do not have love, I am only a resounding gong or a clanging cymbal. If I have the gift of prophecy and can fathom all mysteries and all knowledge, and if I have a faith that can move mountains, but do not have love, I am nothing. If I give all I possess to the poor and give over my body to hardship that I may boast, but do not have love, I gain nothing.\n\nLove is patient, love is kind. It does not envy, it does not boast, it is not proud. It does not dishonor others, it is not self-seeking, it is not easily angered, it keeps no record of wrongs. Love does not delight in evil but rejoices with the truth. It always protects, always trusts, always hopes, always perseveres.\n\nLove never fails. But where there are prophecies, they will cease; where there are tongues, they will be stilled; where there is knowledge, it will pass away. For we know in part and we prophesy in part, but when completeness comes, what is in part disappears. When I was a child, I talked like a child, I thought like a child, I reasoned like a child. When I became a man, I put the ways of childhood behind me. For now we see only a reflection as in a mirror; then we shall see face to face. Now I know in part; then I shall know fully, even as I am fully known.\n\nAnd now these three remain: faith, hope and love. But the greatest of these is love.\n";
uint32_t addr = 0; 
int errcount = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  Serial.println("Hi, there.");
  Serial.println("Let's start.");

  sram_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t val;
  const uint16_t blocksize = (1<<7);

  sram_bank(addr >> 16 & 1);
  
  //addr = random(0, 0x20000);
  if ( addr > 0x1ffff ) {
    Serial.print("completed with ");
    Serial.print(errcount);
    Serial.println(" erros.");
    while(1);  
  }
  Serial.print(addr, HEX);
  Serial.print(" -- ");
  Serial.print(addr+blocksize-1, HEX);
  Serial.print(": ");

  randomSeed(addr);
  uint8_t crc = 0;
  for(uint16_t i = 0; i < blocksize; i++) {
    val = random(0xaa, 256);
    sram_write(addr+i, val);
    crc8(val, crc);
  }
  Serial.print(crc, HEX);
  Serial.print(", ");
  //delay(10);

  uint8_t crc2 = 0;
  for(uint32_t i = 0; i < blocksize; i++) {
    val = sram_read(addr+i);
    crc8(val, crc2);
  }
  Serial.print(crc2, HEX);
  crc8(crc, crc2);
  if ( !crc2 ) {
    Serial.println("  ok ");
  } else {
    errcount++;
    Serial.println(" error!");
    randomSeed(addr);
    crc = 0;
    for(uint16_t i = 0; i < blocksize; i++) {
      val = random(0xaa, 256);
      Serial.print(val, HEX);
      Serial.print(' ');
      sram_write(addr+i, val);
      crc8(val, crc);
    }
    Serial.print(": ");
    Serial.println(crc, HEX);
  
    crc2 = 0;
    for(uint32_t i = 0; i < blocksize; i++) {
      val = sram_read(addr+i);
      Serial.print(val, HEX);
      Serial.print(' ');
      crc8(val, crc2);
    }
    Serial.print(": ");
    Serial.println(crc2, HEX);
    delay(10000);
  }
  addr += blocksize;
  //delay(10);
}

