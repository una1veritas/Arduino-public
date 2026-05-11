#include <SPI.h>
//#include "SPISRAM.h"
#include "shift_registers.h"

const int SRAM_CS = 10;
const int ShiftReg_CS = A3;

ShiftRegisters bus24(ShiftReg_CS, 24, ShiftRegisters::LSB_FIRST);

struct DataBus {
  volatile uint8_t & high2out = PORTB;
  volatile uint8_t & high2in = PINB;
  volatile uint8_t & low6out = PORTD;
  volatile uint8_t & low6in = PIND;
  const uint8_t high2mask = 0x3;
  const uint8_t low6mask = 0xfc;

  uint8_t read() {
    DDRB &= ~high2mask;  // set 0, input
    DDRD &= ~low6mask;  // set 0
    return (low6in >> 2) | (high2in << 6);
  }

  void write(uint8_t val) {
    DDRB |= high2mask;  // set 1, output
    DDRD |= low6mask;  // set 1
    high2out |= val >> 6;
    low6out |= val << 2;
  }
} databus;

void setup() {
  // put your setup code here, to run once:
  pinMode(SRAM_CS, OUTPUT);
  digitalWrite(SRAM_CS, HIGH);

Serial.begin(115200);

  bus24.begin();
  SPI.begin();

unsigned long swatch = millis();
  for (uint32_t i = 0; i < 0x20000; ++i) {
    bus24.write32( i );
    uint8_t val = databus.read();
    databus.write(val);
  }
  Serial.println(millis() - swatch);
  bus24.write32( 0 );
}

void loop() {
  // put your main code here, to run repeatedly:

}
