#include <SoftwareSerial.h>

const byte LATCH_H_CLK = 13;  // PB5
const byte LATCH_L_CLK = 12;  // PB4
const byte SRAM_OE = 11;    // PB3 
const byte SRAM_CS = 10;    // PB2
const byte SRAM_WE = 9;    // PB1
const byte SD_CS = 8;    // PB0

const uint8_t SRAM_WE_BIT = 1<<1;
const uint8_t SRAM_OE_BIT = 1<<3;
const uint8_t SRAM_CS_BIT = 1<<2;
const uint8_t LATCH_L_BIT = 1<<4;
const uint8_t LATCH_H_BIT = 1<<5;
volatile uint8_t * CONTROL = & PORTB;

void select(void) {
  *CONTROL &= ~SRAM_CS_BIT;
}

void deselect(void) {
  *CONTROL |= SRAM_CS_BIT;
}


void addr_out(uint32_t addr) {
  // output addr to ADDR_BUS
  DDRD |= 0xff;

  PORTD = addr;
  addr >>= 8;
  
  *CONTROL &= ~LATCH_L_BIT;
  *CONTROL |= LATCH_L_BIT;
  
  PORTD = addr;

  *CONTROL &= ~LATCH_H_BIT;
  *CONTROL |= LATCH_H_BIT;

  addr >>= 8;
  PORTB &= ~(1<<0);
  PORTD |= (addr & 1);
}

void data_out(uint8_t val) {
  // output val to DATA_BUS
  PORTD = val;
}

uint8_t data_in(void) {
  uint8_t val;

  DDRD = 0x00;
  __asm__ __volatile("nop");

  *CONTROL &= ~SRAM_OE_BIT;
  __asm__ __volatile("nop");
  __asm__ __volatile("nop");
  val = PIND;
  *CONTROL |= SRAM_OE_BIT;

  return val;
}

void write(uint32_t addr, uint8_t val) {
  addr_out(addr);
  /*
  for(byte b = 0; b < DATA_BUSWIDTH; b++, val >>= 1 ) {
    if ( val & 1 )
      digitalWrite(DATA[b], HIGH);
    else
      digitalWrite(DATA[b], LOW);
  }
  */
  data_out(val);
  *CONTROL &= ~SRAM_WE_BIT;
  *CONTROL |= SRAM_WE_BIT;
}

uint8_t read(uint32_t addr) {
  addr_out(addr);
  /*
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
*/
  return data_in();
}

char text[] = "If I speak in the tongues of men or of angels, but do not have love, I am only a resounding gong or a clanging cymbal. If I have the gift of prophecy and can fathom all mysteries and all knowledge, and if I have a faith that can move mountains, but do not have love, I am nothing. If I give all I possess to the poor and give over my body to hardship that I may boast, but do not have love, I gain nothing.\n\nLove is patient, love is kind. It does not envy, it does not boast, it is not proud. It does not dishonor others, it is not self-seeking, it is not easily angered, it keeps no record of wrongs. Love does not delight in evil but rejoices with the truth. It always protects, always trusts, always hopes, always perseveres.\n"; //\nLove never fails. But where there are prophecies, they will cease; where there are tongues, they will be stilled; where there is knowledge, it will pass away. For we know in part and we prophesy in part, but when completeness comes, what is in part disappears. When I was a child, I talked like a child, I thought like a child, I reasoned like a child. When I became a man, I put the ways of childhood behind me. For now we see only a reflection as in a mirror; then we shall see face to face. Now I know in part; then I shall know fully, even as I am fully known.\n\nAnd now these three remain: faith, hope and love. But the greatest of these is love.\n";
const uint16_t len = strlen(text);

SoftwareSerial serial(14, 15);

void setup(void) {
  serial.begin(19200);
  serial.println("Hello, there.");
  serial.println(len);

  DDRB |= ( (1<<0) | SRAM_WE_BIT | SRAM_OE_BIT | SRAM_CS_BIT | LATCH_L_BIT | LATCH_H_BIT);
  pinMode(SD_CS, OUTPUT);
  /*
  digitalWrite(LATCH_L_CLK, HIGH);
  digitalWrite(LATCH_H_CLK, HIGH);
  digitalWrite(SRAM_CS, HIGH);
  digitalWrite(SRAM_OE, HIGH);
  digitalWrite(SRAM_WE, HIGH);
  */
  digitalWrite(SD_CS, HIGH);

  DDRC |= 1;
  DDRD |= 0xff;
}

void loop(void) {
  byte val;
  uint32_t base, addr;

  base = random(0, 0x1ffff);

  // WRITE
  serial.print("write @ H");
  serial.print(base, HEX);
  serial.print(": ");

  long swatch = micros();
  select();
  for(uint16_t offset = 0; offset < 512; offset++) {
    addr = base + offset;
    val = text[addr & 0x1ff];

  /*
    if ( isprint(val) ) {
      Serial.print((char)val);
    } else {
      Serial.print(" ");
      Serial.print(val>>8 & 0x0f, HEX);
      Serial.print(val & 0x0f, HEX);
      Serial.print(" ");
    }
    */
    write(addr, val);
  }
  deselect();
  swatch = micros() - swatch;
  serial.println(swatch);

  // READ
  serial.print("read  @ H");
  serial.print(base, HEX);
  serial.print(": ");
  uint16_t diffs = 0;

  swatch = micros();
  select();
  for(uint16_t offset = 0; offset < 512; offset++) {
    addr = base + offset;
    val = read(addr);
    if ( val != text[addr & 0x1ff] )
      diffs++;
  }
  deselect();
  swatch = micros() -swatch;
  serial.println(swatch);
  serial.print("errors = ");
  serial.println(diffs);
  serial.println();
  
  delay(1000);
}

