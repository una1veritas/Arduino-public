#include <SPI.h>
//#include "SPISRAM.h"
#include "spishiftregisters.h"

const int SRAM_WE = A0;
const int SRAM_CE = A1;
const int SRAM_OE = A2;
const int ShiftReg_CS = A3;
const int SPISRAM_CS = 10;

SPIShiftRegisters bus24(ShiftReg_CS, 24, SPIShiftRegisters::LSB_FIRST);

inline void delay_62ns() { __asm__ __volatile__ ("nop\n\t"); }  // about 62.7 ns

inline void sram_select() { digitalWrite(SRAM_CE, LOW); }
inline void sram_deselect() { digitalWrite(SRAM_CE, HIGH); }
inline void output_enable() { digitalWrite(SRAM_OE, LOW); }
inline void output_disable() { digitalWrite(SRAM_OE, HIGH); }
inline void write_enable() { digitalWrite(SRAM_WE, LOW); }
inline void write_disable() { digitalWrite(SRAM_WE, HIGH); }

uint8_t read_databus() {
  return (PIND >> 2) | (PINB << 6);
}

void write_databus(const uint8_t val) {
  PORTD = (PORTD & 0x03) | (val << 2);
  PORTB = (PORTB & 0xfc) | (val >> 6);
}

void set_databus_mode(const uint8_t inout) {
  if (inout == INPUT) {
    DDRB &= ~0x03;  // set 0, input
    DDRD &= ~0xfc;  // set 0
  } else if (inout == OUTPUT) {
    DDRB |= 0x03;  // set 1, output
    DDRD |= 0xfc;  // set 1
  }
}

void set_address(const uint32_t & addr) {
  bus24.write32(addr);
}

uint8_t read(const uint32_t & addr) {
  set_databus_mode(INPUT);
  set_address(addr);
  sram_select();
  output_enable();
  delay_62ns();
  uint8_t val = read_databus();
  output_disable();
  sram_deselect();
  return val;
}

uint8_t write(const uint32_t & addr, const uint8_t data) {
  set_databus_mode(INPUT);
  set_address(addr);
  sram_select();
  write_enable();
  delay_62ns();
  uint8_t val = read_databus();
  delay_62ns();
  write_disable();
  sram_deselect();
  return val;
}

void sram_begin() {
  sram_select();
  pinMode(SRAM_CE, OUTPUT);
  output_enable();
  pinMode(SRAM_OE, OUTPUT);
  write_enable();
  pinMode(SRAM_WE, OUTPUT);
}

void setup() {
  // put your setup code here, to run once:

  sram_begin();
  bus24.begin();
  digitalWrite(SPISRAM_CS, HIGH);
  digitalWrite(SPISRAM_CS, OUTPUT);
  SPI.begin();
  Serial.begin(115200);

  Serial.println();
  Serial.println(read(0), HEX);
  write(0, 0x55);
  Serial.println(read(0), HEX);
  Serial.println();

}

void loop() {
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
}
