/* Arduino Mega 1280/2560 */

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

const uint8 SRAM_CS_PIN = 2; 
const uint8 SRAM_OE_PIN = 3; 
const uint8 SRAM_WE_PIN = 4; 
//const uint8 SRAM_A16_PIN = 38; 


char text[] = "About the ninth hour, Jesus cried out in a loud voice, \"Eli, Eli, lema sabachthani?\" which means, \"My God, My God, why have you forsaken Me?\" When some of those standing there heard this, they said, \"He is calling Elijah.\"";
char text2[] = "Love is patient, love is kind. It does not envy, it does not boast, it is not proud. It does not dishonor others, it is not self-seeking, it is not easily angered, it keeps no record of wrongs. Love does not delight in evil but rejoices with the truth. It always protects, always trusts, always hopes, always perseveres. \nLove never fails. But where there are prophecies, they will cease; where there are tongues, they will be stilled; where there is knowledge, it will pass away. For we know in part and we prophesy in part, but when completeness comes, what is in part disappears. When I was a child, I talked like a child, I thought like a child, I reasoned like a child. When I became a man, I put the ways of childhood behind me. For now we see only a reflection as in a mirror; then we shall see face to face. Now I know in part; then I shall know fully, even as I am fully known. \nAnd now these three remain: faith, hope and love. But the greatest of these is love.";
uint16 textlen = strlen(text);
uint16 textlen2 = strlen(text2);
uint16 counter = 0;

void setup() {
  Serial.begin(38400);
  Serial.println("Hello. SRAM test.");
  Serial.println(text2);
  Serial.println(textlen2);
  
  // CONTROL
  pinMode(SRAM_CS_PIN, OUTPUT);
  pinMode(SRAM_OE_PIN, OUTPUT);
  pinMode(SRAM_WE_PIN, OUTPUT);
  digitalWrite(SRAM_CS_PIN, HIGH);
  digitalWrite(SRAM_OE_PIN, HIGH);
  digitalWrite(SRAM_WE_PIN, HIGH);

  // PORT MODE
  DDRA = 0xFF;  // A0-A7
  DDRC = 0xFF;  // A8-A15
  DDRL |= 1<<0; // A16 (PL0)
  
  DDRK = 0xFF; // D0 - D7

  // clear A16 bit
  PORTL &= ~(1<<0);
}


void loop() {
  uint8 val;
  uint32 addr = random() & 0x1ffff; // base address

  digitalWrite(SRAM_CS_PIN, LOW);
  for(uint16 i = 0; i < textlen2; i++) {
    val = text2[i];
    PORTA = (addr + i) & 0xff;
    PORTC = (addr + i) >> 8 & 0xff;
    PORTL = (PORTL & ~(1<<0)) | ((addr + i) & (1<<16));
    DDRK = 0xFF;
    PORTK = val;
    digitalWrite(SRAM_WE_PIN, LOW);
    __nop();
    digitalWrite(SRAM_WE_PIN, HIGH);
//    DDRK = 0x00;
  }
  digitalWrite(SRAM_CS_PIN, HIGH);
  

  digitalWrite(SRAM_CS_PIN, LOW);
  Serial.print(addr, HEX);
  Serial.print(": ");
  for(uint16 i = 0; i < textlen2; i++) {
    PORTA = (addr + i) & 0xff;
    PORTC = (addr + i)>>8 & 0xff;
    PORTL = (PORTL & ~(1<<0)) | ((addr + i) & (1<<16));
    DDRK = 0x00;
    digitalWrite(SRAM_OE_PIN, LOW);
    val = PINK;
    digitalWrite(SRAM_OE_PIN, HIGH);
//    DDRK = 0xff;
    if ( isprint(val) ) {
      Serial.print((char)val);
    } else {
      Serial.print("[");
      Serial.print(val, HEX);
      Serial.print("]");
    }
  }
  Serial.println();
  digitalWrite(SRAM_CS_PIN, HIGH);

  delay(2500);

  counter++;
}

