
const byte LATCH_H_CLK = 13;  // PB5
const byte LATCH_L_CLK = 12;  // PB4
const byte SRAM_OE = 11;    // PB3
const byte SRAM_CS = 10;    // PB2
const byte SRAM_WE = 9;    // PB1
const byte SD_CS = 8;    // PB0

const byte ADDR[] = {
  14, 15, 16, 17, 
  4, 5, 6, 7,
  14, 15, 16, 17, 
  4, 5, 6, 7,
  3, 
};
const byte ADDR_BUSWIDTH = 17;

const byte DATA[] = {
  14, 15, 16, 17,
  4, 5, 6, 7,
};
const byte DATA_BUSWIDTH = 8;

void select(void) {
  digitalWrite(SRAM_CS, LOW); 
}

void deselect(void) {
  digitalWrite(SRAM_CS, HIGH); 
}

void latch_low(void) {
  digitalWrite(LATCH_L_CLK, LOW);
  digitalWrite(LATCH_L_CLK, HIGH);
}

void latch_high(void) {
  digitalWrite(LATCH_H_CLK, LOW);
  digitalWrite(LATCH_H_CLK, HIGH);
}

void addr_out(uint32_t addr) {
  // output addr to ADDR_BUS
  for(byte b = 0; b < ADDR_BUSWIDTH; b++, addr >>= 1 ) {
    pinMode(ADDR[b], OUTPUT);
    digitalWrite(ADDR[b], addr & 1);
    if ( b == 7 ) 
      latch_low();
    else if ( b == 15 )
      latch_high();
  }  
}

void write(uint32_t addr, uint8_t val) {
  addr_out(addr);
  
  for(byte b = 0; b < DATA_BUSWIDTH; b++, val >>= 1 ) {
    if ( val & 1 )
      digitalWrite(DATA[b], HIGH);
    else
      digitalWrite(DATA[b], LOW);
  }

  digitalWrite(SRAM_WE, LOW);
  __asm__ __volatile__ ("nop");
  digitalWrite(SRAM_WE, HIGH);
}

uint8_t read(uint64_t addr) {
  uint8_t val, sbit;
  addr_out(addr);

  for(byte b = 0; b < DATA_BUSWIDTH; b++) {
    pinMode(DATA[b], INPUT);
  }
  
  digitalWrite(SRAM_OE, LOW);
  val = 0;
  sbit = 1;
  for(byte b = 0; b < DATA_BUSWIDTH; b++, sbit <<= 1) {
    if ( digitalRead(DATA[b]) )
      val |= sbit;
  }
  digitalWrite(SRAM_OE, HIGH);

  return val;
}

char text[] = "If I speak in the tongues of men or of angels, but do not have love, I am only a resounding gong or a clanging cymbal. If I have the gift of prophecy and can fathom all mysteries and all knowledge, and if I have a faith that can move mountains, but do not have love, I am nothing. If I give all I possess to the poor and give over my body to hardship that I may boast, but do not have love, I gain nothing.\n\nLove is patient, love is kind. It does not envy, it does not boast, it is not proud. It does not dishonor others, it is not self-seeking, it is not easily angered, it keeps no record of wrongs. Love does not delight in evil but rejoices with the truth. It always protects, always trusts, always hopes, always perseveres.\n"; //\nLove never fails. But where there are prophecies, they will cease; where there are tongues, they will be stilled; where there is knowledge, it will pass away. For we know in part and we prophesy in part, but when completeness comes, what is in part disappears. When I was a child, I talked like a child, I thought like a child, I reasoned like a child. When I became a man, I put the ways of childhood behind me. For now we see only a reflection as in a mirror; then we shall see face to face. Now I know in part; then I shall know fully, even as I am fully known.\n\nAnd now these three remain: faith, hope and love. But the greatest of these is love.\n";
const uint16_t len = strlen(text);

void setup(void) {
  Serial.begin(19200);
  Serial.println("Hello, there.");

  pinMode(LATCH_L_CLK, OUTPUT);
  pinMode(LATCH_H_CLK, OUTPUT);
  pinMode(SRAM_CS, OUTPUT);
  pinMode(SRAM_OE, OUTPUT);
  pinMode(SRAM_WE, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(LATCH_L_CLK, HIGH);
  digitalWrite(LATCH_H_CLK, HIGH);
  digitalWrite(SRAM_CS, HIGH);
  digitalWrite(SRAM_OE, HIGH);
  digitalWrite(SRAM_WE, HIGH);
  digitalWrite(SD_CS, HIGH);
  
  for(byte b = 0; b < ADDR_BUSWIDTH; b++)
    pinMode(ADDR[b], OUTPUT);
}

void loop(void) {
  byte val;
  uint32_t base, addr;

  base = random(0, 0x1ffff);

  // WRITE SEQ
  Serial.print("write @ h");
  Serial.print(base, HEX);
  Serial.print(": ");

  for(uint16_t offset = 0; offset < 256; offset++) {
  addr = base + offset;
  val = text[addr % len];

  if ( isprint(val) ) {
    Serial.print((char)val);
  } else {
    Serial.print(" ");
    Serial.print(val>>8 & 0x0f, HEX);
    Serial.print(val & 0x0f, HEX);
    Serial.print(" ");
  }
  /*
  // output count to ADDR_BUS
  for(byte b = 0; b < ADDR_BUSWIDTH; b++) {
    pinMode(ADDR[b], OUTPUT);
    digitalWrite(ADDR[b], count>>b & 1);
  }
  delay(500);
  latch();
  delay(1000);
  for(byte b = 0; b < DATA_BUSWIDTH; b++) {
    if ( val >> b & 1 )
      digitalWrite(DATA[b], HIGH);
    else
      digitalWrite(DATA[b], LOW);
  }
  delay(1000);
  digitalWrite(SRAM_CS, LOW);
  digitalWrite(SRAM_WE, LOW);
  delay(10);
  digitalWrite(SRAM_WE, HIGH);
  digitalWrite(SRAM_CS, HIGH);
  */
  select();
  write(addr, val);
  deselect();
  }
  Serial.println();
  /*
  delay(1000);
  // READ SEQ
  */
  Serial.print("read  @ h");
  Serial.print(base, HEX);
  Serial.print(": ");
  uint16_t diffs = 0;
  for(uint16_t offset = 0; offset < 256; offset++) {
    addr = base + offset;
  /*
  // output count to ADDR_BUS
  for(byte b = 0; b < ADDR_BUSWIDTH; b++) {
    pinMode(ADDR[b], OUTPUT);
    digitalWrite(ADDR[b], count>>b & 1);
  }
  delay(500);
  latch();

  delay(1000);
  for(byte b = 0; b < DATA_BUSWIDTH; b++) {
    pinMode(DATA[b], INPUT);
  }
  
  delay(1000);
  digitalWrite(SRAM_CS, LOW);
  digitalWrite(SRAM_OE, LOW);
  delay(10);
  val = 0;
  for(byte b = 0; b < DATA_BUSWIDTH; b++) {
    if ( digitalRead(DATA[b]) )
      val |= 1<<b;
  }
  digitalWrite(SRAM_OE, HIGH);
  digitalWrite(SRAM_CS, HIGH);
  */

  select();
  val = read(addr);
  deselect();
  if ( val != text[addr%len] )
    diffs++;

  if ( isprint(val) ) {
    Serial.print((char)val);
  } else {
    Serial.print(" ");
    Serial.print(val>>8 & 0x0f, HEX);
    Serial.print(val & 0x0f, HEX);
    Serial.print(" ");
  }
  /*
  for(byte b = 0; b < DATA_BUSWIDTH; b++) {
    pinMode(DATA[b], OUTPUT);
    if ( val>>b & 1 ) 
      digitalWrite(DATA[b], HIGH);
    else
      digitalWrite(DATA[b], LOW);
  }
  delay(1000);
    for(byte b = 0; b < DATA_BUSWIDTH; b++) {
      digitalWrite(DATA[b], LOW);
    }
    */
  }
  Serial.println();  
  Serial.print("errors = ");
  Serial.println(diffs);
  Serial.println();
  delay(1000);
}

