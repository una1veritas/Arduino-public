#include <SPI.h>
//#include "SPISRAM.h"
#include "spishiftregisters.h"

const int SPISRAM_CS = 10;

struct SRAM {
  const int SRAM_WE = A0;
  const int SRAM_CE = A1;
  const int SRAM_OE = A2;
  const int ShiftReg_CS = A3;

  SPIShiftRegisters addrbus;

SRAM() : addrbus(SPIShiftRegisters(ShiftReg_CS, 24, SPIShiftRegisters::LSB_FIRST)){}

  static const uint8_t PORTD_MASK = 0xfc;   // high 6 bits
  static const uint8_t PORTB_MASK = 0x03;   // low 2 bits

  inline static void delay_62ns() {
    __asm__ __volatile__("nop\n\t");
  }  // about 62.7 ns

  inline void select() {
    digitalWrite(SRAM_CE, LOW);
  }
  inline void deselect() {
    digitalWrite(SRAM_CE, HIGH);
  }
  inline void output_enable() {
    digitalWrite(SRAM_OE, LOW);
  }
  inline void output_disable() {
    digitalWrite(SRAM_OE, HIGH);
  }
  inline void write_enable() {
    digitalWrite(SRAM_WE, LOW);
  }
  inline void write_disable() {
    digitalWrite(SRAM_WE, HIGH);
  }

  void begin() {
    addrbus.begin();
    select();
    pinMode(SRAM_CE, OUTPUT);
    output_enable();
    pinMode(SRAM_OE, OUTPUT);
    write_enable();
    pinMode(SRAM_WE, OUTPUT);
  }

  void set_databus_mode(const uint8_t inout) {
    if (inout == INPUT) {
      PORTB |= PORTB_MASK;  // set 1 -> weak pull-up
      PORTD |= PORTD_MASK; // pull-up
      DDRB  &= ~PORTB_MASK;  // set 0 -> input
      DDRD  &= ~PORTD_MASK;  //~0xfc;  // set 0
    } else if (inout == OUTPUT) {
      DDRB |= PORTB_MASK;  // set 1 -> output
      DDRD |= PORTD_MASK;  // set 1
    }
  }

  uint8_t get_databus() {
    return (PINB & PORTB_MASK) | (PIND & PORTD_MASK);
  }

  void set_databus(const uint8_t val) {
    PORTD &= ~PORTD_MASK; 
    PORTD |= val & PORTD_MASK;
    PORTB &= ~PORTB_MASK;
    PORTB |= val & PORTB_MASK;
  }

  void set_address(const uint32_t& addr) {
    addrbus.write32(addr);
  }

  uint8_t read(const uint32_t& addr) {
    set_databus_mode(INPUT);
    set_address(addr);
    select();
    output_enable();
    delay_62ns();
    uint8_t val = get_databus();
    output_disable();
    deselect();
    return val;
  }

  uint8_t write(const uint32_t& addr, const uint8_t data) {
    set_databus_mode(OUTPUT);
    set_address(addr);
    set_databus(data);
    select();
    delay_62ns();
    write_enable();
    delay_62ns();
    write_disable();
    deselect();
    return data;
  }

};

SRAM hm628128;

void setup() {
  // put your setup code here, to run once:

  hm628128.begin();
  digitalWrite(SPISRAM_CS, HIGH);
  pinMode(SPISRAM_CS, OUTPUT);
  SPI.begin();
  Serial.begin(115200);

  Serial.println();
  uint32_t addr = 0x0230;
  for(uint32_t val = 0; val < 0x100; ++val) {
    Serial.print("addr :"); Serial.print(addr, HEX);
    hm628128.write(addr, 0x0);
    Serial.print(", read :"); Serial.print(hm628128.read(addr), HEX);
    hm628128.write(addr, val);
    Serial.print(", written: "); Serial.print(val, HEX);
    Serial.print(", read: "); Serial.println(hm628128.read(addr), HEX);
  }
  Serial.println();
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
}
