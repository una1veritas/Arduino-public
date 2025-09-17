#include <SPI.h>

//Pin connected to latch pin (ST_CP) of 74HC595
const int CS = 53;//latchPin = 53; --- must be controlled by user
//Pin connected to clock pin (SH_CP) of 74HC595
const int CLK = 52; //clockPin = 52; --- controlled by SPI module.
////Pin connected to Data in (DS) of 74HC595
const int COPI = 51; //dataPin = 51; --- controlled by SPI module.
//https://wiki.dfrobot.com/3-Wire_LED_Module__SKU_DFR0090_

byte ascii7seg(byte ch) {
  const static uint8_t numeric7[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff };
  const static uint8_t alpha7[] = { 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xc2, 0x89, 0x79, 0xe1,
    0x09 /* 0x89*/, 0xc7, 0xbb /* 0xaa*/, 0xab /* 0xc8*/, 0xa3, 0x8c, 0x98, 0xaf /* 0xce */, 0x92 /*0x9b*/,
    0x87, 0xc1, 0xc1 /* 0xe3*/, 0x86 /* 0xd5 */, 0xb6, 0x91, 0x24 /* 0xb8*/,
  };  //a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z
  if ('0' <= ch and ch <= '9') {
    return numeric7[ch - '0'];
  }
  switch(ch){
    case '.':
    case ',':
      return 0x7f;
      break;
    case ' ':
      return 0xff;
      break;
    case 'i':
      return 0x7b;
      break;
    case 'o':
      return 0xa3;
      break;
    case 'u':
      return 0xe3;
      break;
  }
  ch = toupper(ch);
  if ('A' <= ch and ch <= 'Z') {
    return alpha7[ch - 'A'];
  } 
  return 0xff;
}

// 0x01 --- top
// 0x02 --- right upper vertical bar
// 0x04 --- right lower
// 0x08 --- bottom
// 0x10 --- left lower
// 0x20 --- left upper
// 0x40 --- middle
// 0x80 --- dot
uint8_t byte7seg(uint8_t b) {
  const static uint8_t numeric7[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90 };
  const static uint8_t alpha7[] = { 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xc2, 0x89, 0x79, 0xe1,
    0x09 /* 0x89*/, 0xc7, 0xbb /* 0xaa*/, 0xab /* 0xc8*/, 0xa3, 0x8c, 0x98, 0xaf /* 0xce */, 0x92 /*0x9b*/,
    0x87, 0xc1, 0xc1 /* 0xe3*/, 0x86 /* 0xd5 */, 0xb6, 0x91, 0x24 /* 0xb8*/,
  };  //a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z
  const static uint8_t spc_dot[] = { 0xff, 0x7f };
  if (b < 10)
    return numeric7[b];
  else if (b < 16)
    return alpha7[b - 10];
  else if (b == 0x80)
    return 0x7f;
  else
    return 0xff;
} 

unsigned long counter = 0;
char buf[16];
char sym[5] = {'r', 'u', 'i', 'o', ' '};
void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(CS, OUTPUT);
  // SPI.setClockDivider(SPI_CLOCK_DIV4);  -- div4 seems to be the default clock divider value
  SPI.begin();
}

void loop() {
  long swatch = micros();
  unsigned char d;
  snprintf(buf, 9, "%04X %c%02X", uint16_t(counter>>8), sym[uint8_t((counter>>16)%5)], uint8_t(counter>>11) );
  digitalWrite(CS, LOW);
  for (int i = 8; i > 0; ) {
    // write to the shift register with the correct bit set high:
    // shift the bits out:
    //shiftOut(dataPin, clockPin, MSBFIRST, byte7seg(d));
    SPI.transfer(ascii7seg(buf[--i]));
    // turn on the output so the LEDs can light up:
  }
    digitalWrite(CS, HIGH);
  //counter = micros() - swatch;
  counter++;
  //delay(100);
}
