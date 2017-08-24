#include "SRAM_mega.h"

char text[] = "If I speak in the tongues of men or of angels, but do not have love, I am only a resounding gong or a clanging cymbal. If I have the gift of prophecy and can fathom all mysteries and all knowledge, and if I have a faith that can move mountains, but do not have love, I am nothing. If I give all I possess to the poor and give over my body to hardship that I may boast, but do not have love, I gain nothing.\n\nLove is patient, love is kind. It does not envy, it does not boast, it is not proud. It does not dishonor others, it is not self-seeking, it is not easily angered, it keeps no record of wrongs. Love does not delight in evil but rejoices with the truth. It always protects, always trusts, always hopes, always perseveres.\n\nLove never fails. But where there are prophecies, they will cease; where there are tongues, they will be stilled; where there is knowledge, it will pass away. For we know in part and we prophesy in part, but when completeness comes, what is in part disappears. When I was a child, I talked like a child, I thought like a child, I reasoned like a child. When I became a man, I put the ways of childhood behind me. For now we see only a reflection as in a mirror; then we shall see face to face. Now I know in part; then I shall know fully, even as I am fully known.\n\nAnd now these three remain: faith, hope and love. But the greatest of these is love.\n";
SRAM sram;

long addr = 0;
uint16_t length = strlen(text);

void setup() {
  Serial.begin(38400);
  Serial.println("Started.");
  
	sram.begin();

}

void loop() {
  byte val;
  uint16_t offset = random(0, length - 128);

  addr = random(0, 0x1ffff);
  Serial.print("base addr: ");
  Serial.println(addr, HEX);
  Serial.print("write: \t");
  sram.enable();
  for(int i = 0; i < 128; i++) {
    val = text[offset+i]; //random(' ', 'Z');
    Serial.print((char)val);
    //Serial.print(", ");
    sram.bank(addr>>16 & 1);
    sram.write(addr+i, val);
  }
  Serial.println();
  Serial.print("read: \t");
  for(int i = 0; i < 128 ; i++) {
    val = sram.read(addr+i);
    Serial.print((char)val);
    //Serial.print(", ");
  }
  Serial.println();
  sram.disable();
  Serial.println();

  delay(500);
}

