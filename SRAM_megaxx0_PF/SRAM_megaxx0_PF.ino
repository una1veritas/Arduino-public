#include "sram.h"

/* sram_avr.c functions */
/* atmegaxxx0 PA0 -- PC7, PL0 for A0 - A16 
   and PF0 - PF7 for D0 - D7 */


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
const uint16_t blocksize = (1<<10);
uint8_t readout[blocksize];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi, there.");
  Serial.println("Let's start.");

  sram_bus_setup();
  sram_enable();

  randomSeed(analogRead(8) + (analogRead(9)<<8));

  uint8_t val;
  
  for( addr = 0; addr < 0x20000; addr += blocksize) {
    long blockseed = random(0, 32000);
    blockerrcount = 0;

    Serial.print(addr, HEX);
    randomSeed(blockseed);
    for(uint16_t i = 0; i < blocksize; i++) {
      sram_write(addr+i, random(0, 256));
    }
  
    randomSeed(blockseed);
    for(uint32_t i = 0; i < blocksize; i++) {
      readout[i] = sram_read(addr+i);
      if ( readout[i] != random(0,256) ) {
        blockerrcount++;
      }
    }
    if ( blockerrcount == 0 ) {
      Serial.print(" -- ");
      Serial.println(addr+blocksize-1, HEX);
    } else {
      errcount += blockerrcount;
      Serial.println(" error!");
      unsigned int count = 0;
      for(uint16_t i = 0; i < blocksize; i++) {
        Serial.print(readout[i], HEX);
        Serial.print(' ');
      }
      Serial.println();
      break;
    }
  }
  Serial.println();
  Serial.print(errcount);
  Serial.println(" erros.");
  Serial.print("completed.");
}
  
void loop() {
}

