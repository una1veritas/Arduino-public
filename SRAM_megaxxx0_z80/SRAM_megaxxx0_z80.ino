#include "sram_mega100_PF.h"
#include "z80_io.h"
/* sram_avr.c functions */
/* atmegaxxx0 PA0 -- PC7, PL0 for A0 - A16 
   and PF0 - PF7 for D0 - D7 */

void sram_init() {
  ADDRL_DIR = ADDRL_MASK; // fixed to be OUTPUT
  ADDRH_DIR = ADDRH_MASK; // fixed to be OUTPUT
  ADDRX_DIR |= ADDRX_MASK; // fixed to be OUTPUT
  DATA_DIR = DATA_MASK;
  CONTROL_DIR |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE );
  CONTROL |= ( SRAM_WE | SRAM_OE | SRAM_CS ); //SRAM_ALE);
}

inline void addr_set(uint32_t addr) {
  ADDRL = (uint8_t) addr;
  addr >>= 8;
  ADDRH = (uint8_t) addr;
  addr >>= 8;
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= ((uint8_t)addr) & ADDRX_MASK;
}

inline void sram_bank(uint8_t bk) {
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= (bk & ADDRX_MASK);
}

uint8_t sram_read(uint32_t addr) {
  unsigned char val;
  CONTROL &= ~SRAM_CS;
//  DATA_OUT = 0xff; // clear the pull-ups
  DATA_DIR = 0x00;
  addr_set(addr);
  CONTROL &= ~SRAM_OE;
  __asm__ __volatile__ ("nop");
  val = DATA_IN;
  CONTROL |= SRAM_OE; // valid data remains while
  CONTROL |= SRAM_CS;
  return val;
}

void sram_write(uint32_t addr, uint8_t data) {
  CONTROL &= ~SRAM_CS;
  addr_set(addr);
  DATA_DIR = DATA_MASK;
  DATA_OUT = data;
  CONTROL &= ~SRAM_WE;
  CONTROL |= SRAM_WE;
  CONTROL |= SRAM_CS;
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
int blockerrcount = 0;
int errcount = 0;
const uint16_t blocksize = (1<<9);
uint8_t wdata[blocksize], rdata[blocksize];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi, there.");
  Serial.println("Let's start.");
  
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, LOW);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, LOW);
  pinMode(Z80_CLK_PIN, OUTPUT);
  for(int i = 0; i < 8; i++) {
    digitalWrite(Z80_CLK_PIN, HIGH);
    delay(10);
    digitalWrite(Z80_CLK_PIN, LOW);
    delay(10);
  }

  sram_init();

  randomSeed(analogRead(8) + (analogRead(9)<<8));
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t val;
  
  //addr = random(0, 0x20000);
  if ( addr > 0x1ffff ) {
    Serial.println();
    Serial.print("completed with ");
    Serial.print(errcount);
    Serial.println(" erros.");
    while(1);  
  }

  if ( (addr & 0xfff) == 0 )
    Serial.println();
  Serial.print(addr, HEX);

  blockerrcount = 0;
  for(uint16_t i = 0; i < blocksize; i++) {
    wdata[i] = random(0, 256);
    sram_write(addr+i, wdata[i]);
  }
  //delay(10);

  for(uint32_t i = 0; i < blocksize; i++) {
    rdata[i] = sram_read(addr+i);
    if ( rdata[i] != wdata[i] ) {
      blockerrcount++;
      Serial.print("[");
      Serial.print(addr+i, HEX);
      Serial.print(":");
      Serial.print(wdata[i], HEX);
      Serial.print("/");
      Serial.print(rdata[i], HEX);
      Serial.print("], ");
    }
  }
  if ( blockerrcount == 0 ) {
    Serial.print("; ");
  } else {
    errcount += blockerrcount;
    Serial.println(" error!");
    unsigned int count = 0;
    for(uint16_t i = 0; i < blocksize; i++) {
      Serial.print(wdata[i], HEX);
      Serial.print(' ');
      sram_write(addr+i, wdata[i]);
    }
    Serial.println();
    for(uint32_t i = 0; i < blocksize; i++) {
      val = sram_read(addr+i);
      Serial.print(val, HEX);
      Serial.print(' ');
      if ( wdata[i] != val )
      count++; 
    }
    Serial.println();
    Serial.print("err count on retry = ");
    Serial.println(count);
    Serial.println();
    if ( count > 0 )
      while (1);
    delay(10000);
  }
  addr += blocksize;
  //delay(10);
}

