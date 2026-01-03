#include <LibPrintf.h>

/*
 * 12 ns -- 1/83.3M
 * 55 ns -- 1/18.2M
 * 70 ns -- 1/14.3M
 */
struct SRAM4mb {
  const int _WE = 41; // WR
  const int _OE = 40; // RD
  const int _CE = 39; //ALE

  SRAM4mb() {
    // address bus output
    DDRA = 0xff;
    PORTA = 0x00; // A0 -- A7
    DDRC = 0xff;
    PORTC = 0x00; // A7 -- A15
    DDRK = 0x01;
    PORTK = 0x00; // A16
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
    PORTL = 0xff; // WPULLUP
    DDRL = 0x00; // input
    digitalWrite(_CE, LOW);
    PORTA = *addr++;
    PORTC = *addr++;
    PORTK = (PORTK & 0xfe) | (*addr & 0x01);
    digitalWrite(_OE, LOW);
    //asm("nop");
    val = PINL;
    digitalWrite(_OE, HIGH);
    digitalWrite(_CE, HIGH);
    return val;
  }

  uint8_t write(const uint32_t addr32, const uint8_t val) {
    const uint8_t * addr = (uint8_t *) & addr32;
    DDRL = 0xff; // output
    //PORTL = 0x00; // NO WPULLUP
    digitalWrite(_CE, LOW);
    PORTL = val;
    PORTA = *addr++;
    PORTC = *addr++;
    PORTK = (PORTK & 0xfe) | (*addr & 0x01);
    digitalWrite(_WE, LOW);
    //asm("nop");
    digitalWrite(_WE, HIGH);
    digitalWrite(_CE, HIGH);
  }
  
};

SRAM4mb sram; 
uint8_t rndtbl[257];
uint8_t rbuff[256];

void setup () {
  uint32_t addr32 ;
  uint32_t succs = 0, fails = 0;
	const uint32_t RAM_UB = 0x20000; //(1UL<<17);
	uint8_t rndbyte;

  Serial.begin (57600);
  while (!Serial);
	Serial.println();

	randomSeed(analogRead(0));
	for(uint16_t i = 0; i < 257; ++i) {
	  rndtbl[i] = random(0x100);
    printf("%02x", rndtbl[i]);
		if ( (i & 0x0f) == 0x0f)
			Serial.println();
    else
    Serial.print(" ");
	}
  Serial.println();
  Serial.println("Checking...");
  
	for(addr32 = 0; addr32 < RAM_UB; addr32 += 256) {
		if ( (addr32 & 0x1fff) == 0 ) {
			printf("%06lx\r\n", addr32);
			//Serial.print(addr32, HEX);
		}
    for(uint32_t i = 0; i < 256; ++i) {
      rndbyte = rndtbl[(addr32+i) % 257];
      rbuff[i] = sram.read(addr32+i); 
      sram.write(addr32, rbuff[i]^rndbyte); 
    }
    for(uint32_t i = 0; i < 256; ++i) {
      rndbyte = rndtbl[(addr32+i) % 257];
      uint8_t rval = sram.read(addr32+i); 
      sram.write(addr32, rbuff[i]); 

      if ( rval^rndbyte == rbuff[i] ) {
        succs++;
      } else {
        fails++;
        printf("%06lx: %02x %02x\r\n", addr32+i, rval^rndbyte, rbuff[i]);
      }
    }
  }

  Serial.print("Finisdhed read/write checks from address 0 to ");
  Serial.println(addr32, HEX);
  Serial.print("succs = "); Serial.println(succs);
  Serial.print("fails = "); Serial.println(fails);
}

void loop () {

}
