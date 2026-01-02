#include <LibPrintf.h>

struct SRAM4mb {
  const int _WE = 41;
  const int _OE = 40;
  const int _CE = 39;

  SRAM4mb() {
    // address bus output
    DDRA = 0xff;
    PORTA = 0x00;
    DDRC = 0xff;
    PORTC = 0x00;
    DDRK = 0x07;
    PORTC = 0x00;
    // data bus initially input
    DDRL = 0x00;
    PORTL = 0xff; // WPULLUP
    pinMode(_WE, OUTPUT);
    pinMode(_OE, OUTPUT);
    pinMode(_CE, OUTPUT);
  }

  uint8_t read(const uint32_t addr32) {
    const uint8_t * addr = (uint8_t *) & addr32;
    uint8_t val;
    digitalWrite(_CE, LOW);
    PORTK = (PORTC & 0xf8) | (addr[1] & 0x07);
    PORTC = addr[2];
    PORTA = addr[3];
    DDRL = 0x00; // input
    digitalWrite(_OE, LOW);
    asm("nop");
    val = PINL;
    digitalWrite(_OE, HIGH);
    digitalWrite(_CE, HIGH);
    return val;
  }

  uint8_t write(const uint32_t addr32, const uint8_t val) {
    const uint8_t * addr = (uint8_t *) & addr32;
    digitalWrite(_CE, LOW);
    PORTK = (PORTC & 0xf8) | (addr[1] & 0x07);
    PORTC = addr[2];
    PORTA = addr[3];
    DDRL = 0xff; // output
    PORTL = val;
    digitalWrite(_WE, LOW);
    asm("nop");
    digitalWrite(_WE, HIGH);
    digitalWrite(_CE, HIGH);
  }
  
};

SRAM4mb sram; 
uint8_t rndtbl[256];

void setup () {
  Serial.begin (57600);
  while (!Serial);
	Serial.println();
  uint32_t i ;
  uint32_t succs = 0, fails = 0;
	const uint32_t RAM_UB = 0x80000;
	uint32_t addrmax = 0, addrmin = 1;

	randomSeed(analogRead(0));
	for(uint16_t i = 0; i < 256; ++i) {
		rndtbl[i] = random(0,256);
		Serial.print( (rndtbl[i]>>4) & 0x0f, HEX);
		Serial.print(rndtbl[i] & 0x0f, HEX);
		Serial.print(" ");
		if ( (i & 0x0f) == 0x0f)
			Serial.println();
	}
	bool updated = false;
  for( i = 0; i < RAM_UB; ++i) {
		updated = false;

		uint32_t addr32 = i; // ^ 0x05aa55;
    uint8_t val = rndtbl[i & 0xff];
    sram.write(addr32, val);
    sram.write(RAM_UB, val^0xff);
    uint8_t rval = sram.read(addr32);

		if ( (addr32 & 0xffff) == 0 ) {
			printf("%06lx\r\n", addr32);
			//Serial.print(addr32, HEX);
		}
    if ( val == rval ) {
      succs++;
    } else {
      fails++;
      Serial.print((uint16_t)addr32, HEX);
      Serial.print(": ");
      Serial.print(val, HEX);
      Serial.print(" ");
      Serial.print(rval, HEX);
      Serial.println();
    }
  }

  Serial.print("succs = "); Serial.println(succs);
  Serial.print("fails = "); Serial.println(fails);
  Serial.println(i, HEX);
}

void loop () {

}
