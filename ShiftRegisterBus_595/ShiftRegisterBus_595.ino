#include <SPI.h>
//#include "SPISRAM.h"
#include <ShiftRegister.h>
#include <MCP23S08.h>

const int SPISRAM_CS = 10;



Memory memory(Memory::EEPROM64KBITS);
char buf128[128];
uint8_t data[256];

void memory_read_write_test(uint32_t addrlow = 0, uint32_t addrend = 0);

void(* resetFunc) (void) = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Hello.");
  delay(500);

  digitalWrite(SPISRAM_CS, HIGH);
  pinMode(SPISRAM_CS, OUTPUT);
  SPI.begin();
  memory.begin();

  Serial.println("memory read write test (for SRAM)");
  memory_program_test(0, 0x100);
}

void loop() {
  /*
  // put your main code here, to run repeatedly:
  unsigned long swatch = millis();
  for (uint32_t i = 1; i < 0x1000000; i <<= 1) {
    bus24.write32( i );
    //uint8_t val = databus.read();
    //databus.write(val);
    delay(200);
  }
  Serial.println(millis() - swatch);
  bus24.write32( 0 );
  */
  if (Serial.available() > 0) {
    resetFunc();
  }
}

void memory_read_test(uint32_t addrlow, uint32_t addrhigh) {
  Serial.println("Memory Read Test.");
  addrhigh = (addrhigh > memory.size() ? memory.size() : addrhigh) ;
  Serial.println();
  uint32_t addr;
  for(addr = addrlow; addr < addrhigh; addr += 64) {
    Serial.println(addr, HEX);
    for(uint32_t offset = 0; offset < 64; offset++) {
      uint8_t val = memory.read(addr+offset);
      Serial.print(val, HEX);
      Serial.print(" ");
      if ( (offset & 0x01f) == 0x01f ) {
        Serial.println();
      }
    }
    Serial.println();
  }
  Serial.println(addr, HEX);
}

void memory_program_test(uint32_t addrlow, uint32_t addrhigh) {
  Serial.println("Memory Read Test.");
  addrhigh = (addrhigh > memory.size() ? memory.size() : addrhigh) ;
  Serial.println("Disable software data protection.");
  memory.disable_SDP();
  Serial.println("Writing...");
  uint32_t addr;
  for(addr = addrlow; addr < addrhigh; addr += 64) {
    Serial.println(addr, HEX);
    for(uint32_t offset = 0; offset < 64; offset++) {
      uint8_t val = offset;
      memory.program_byte(addr+offset, val);
      Serial.print(val, HEX);
      Serial.print(" ");
      if ( (offset & 0x01f) == 0x01f ) {
        Serial.println();
      }
    }
    Serial.println();
  }
  Serial.println("Reading...");
  for(addr = addrlow; addr < addrhigh; addr += 64) {
    Serial.println(addr, HEX);
    for(uint32_t offset = 0; offset < 64; offset++) {
      uint8_t val = memory.read(addr+offset);
      Serial.print(val, HEX);
      Serial.print(" ");
      if ( (offset & 0x01f) == 0x01f ) {
        Serial.println();
      }
    }
    Serial.println();
  }
  Serial.println(addr, HEX);
}

void memory_read_write_test(uint32_t addrlow = 0, uint32_t addrend = 0) {
  if ( addrlow == 0 and addrend == 0) {
    addrend = memory.size() ;
  }
  addrend = addrend > memory.size() ? memory.size() : addrend;
  randomSeed(analogRead(5));
  uint32_t totalerrcount = 0;
  Serial.println();
  uint32_t addr;
  Serial.print("Read/Write test from");
  Serial.print(addrlow, HEX); 
  Serial.print(" to ");
  Serial.println(addrend, HEX);
  for(addr = addrlow; addr < addrend; addr += 32) {
    if ( (addr & (0x200 - 1)) == 0 ) {
      snprintf(buf128, 127, "%08lX: ", addr);
      Serial.println(buf128);
    }

    for(uint32_t offset = 0; offset < 32; ++offset) {
      data[offset] = memory.read(addr + offset);
    }
    long seed = millis() ^ analogRead(5);
    randomSeed(seed);
    for(uint32_t offset = 0; offset < 32; ++offset) {
      uint8_t writeval = random(0, 256);
      memory.write(addr + offset, writeval);
    }
    randomSeed(seed);
    uint32_t readout = 0, errcount = 0;
    for(uint32_t offset = 0; offset < 32; ++offset) {
      readout = memory.read(addr + offset);
      if ( readout != random(0, 256) )
        errcount++;
    }
    if ( errcount > 0 ) {
      snprintf(buf128, 127, "%04x: ", addr);
      Serial.println(buf128);
      Serial.print("First Read: ");
      for(uint32_t offset = 0; offset < 32; ++offset) {
        snprintf(buf128, 127, "%02x ", data[offset]);
        Serial.print(buf128);
      }
      Serial.println();
      Serial.print("Then write: ");
      randomSeed(seed);
      for(uint32_t offset = 0; offset < 32; ++offset) {
        uint8_t writeval = random(0, 256);
        snprintf(buf128, 127, "%02x ", writeval);
        Serial.print(buf128);
      }
      Serial.println();
      Serial.print("But read:   ");
      for(uint32_t offset = 0; offset < 32; ++offset) {
        snprintf(buf128, 127, "%02x ", memory.read(addr + offset));
        Serial.print(buf128);
      }
      Serial.println();
      Serial.println();
      Serial.print("occurred errors = ");
      Serial.println(errcount);
      totalerrcount += errcount;
    } else {
      for(uint32_t offset = 0; offset < 32; ++offset) {
        memory.write(addr + offset, data[offset]);
      }
    }
    if ( totalerrcount > 0x400 ) {
      Serial.println("Got too many errors. Abandon.");
      break;
    }
  }
  snprintf(buf128, 127, "%08lX: ", addr);
  Serial.println(buf128);
  Serial.print("total error count = ");
  Serial.println(totalerrcount);
}