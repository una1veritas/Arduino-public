/*
 * memory.h
 *
 *  Created on: 2026/05/16
 *      Author: sin
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "ShiftRegister.h"
#include "MCP23S08.h"

class Memory {
private:
  int MEM_WE;
  int MEM_CE;
  int MEM_OE;
  int ShiftReg_CS;
  int MCP23S08_CS;

  ShiftRegister addrbus;
  MCP23S08 databus;
  uint32_t capacity_bits;

public:
  enum CAPACITY_BITS{
    SRAM32KBITS   =   0x8000UL,   // 4k bytes
    SRAM64KBITS   =  0x10000UL,   // 8k bytes
    SRAM128KBITS  =  0x20000UL,   // 16k bytes
    SRAM832       =  0x40000UL,   // 32kbytes/256k bits
    SRAM256KBITS  =  0x40000UL,   // 32kbytes/256k bits
    SRAM512KBITS  =  0x80000UL,   // 64kbytes/512kbits
    SRAM1MBITS    = 0x100000UL,   // 64kbytes/512kbits
    SRAM8512      = 0x400000UL,   // 64kbytes/512kbits
    SRAM4MBITS    = 0x400000UL,   // 64kbytes/512kbits

    EEPROM64KBITS  = 0x10000UL,
    EEPROM256KBITS = 0x40000UL,
  };

Memory(const uint32_t & capabits,
		uint8_t addrbus_cs = 8,
		uint8_t databus_cs = 9,
		uint8_t CE_pin = A1,
		uint8_t OE_pin = A2,
		uint8_t WE_pin = A0
		) :
			MEM_CE (CE_pin), MEM_OE(OE_pin), MEM_WE(WE_pin),
			addrbus(ShiftRegister(addrbus_cs, 24, ShiftRegister::LSB_FIRST)),
			databus(MCP23S08(databus_cs, 0)),
			capacity_bits(capabits) {
    deselect();
    pinMode(MEM_CE, OUTPUT);
    output_disable();
    pinMode(MEM_OE, OUTPUT);
    write_disable();
    pinMode(MEM_WE, OUTPUT);
}

//  static const uint8_t PORTD_MASK = 0xfc;   // high 6 bits
//  static const uint8_t PORTB_MASK = 0x03;   // low 2 bits
//  static const uint8_t PORTC_MASK = 0x03<<4;   // low 2 bits

  inline static void delay_62ns() {
    __asm__ __volatile__("nop\n\t");
  }  // about 62.7 ns

  inline void select() {
    digitalWrite(MEM_CE, LOW);
  }
  inline void deselect() {
    digitalWrite(MEM_CE, HIGH);
  }
  inline void output_enable() {
    digitalWrite(MEM_OE, LOW);
  }
  inline void output_disable() {
    digitalWrite(MEM_OE, HIGH);
  }
  inline void write_enable() {
    digitalWrite(MEM_WE, LOW);
  }
  inline void write_disable() {
    digitalWrite(MEM_WE, HIGH);
  }

// must be called after SPI has begun
  inline void begin() {
    addrbus.begin();
    databus.begin();
    set_databus_mode(INPUT);
  }

  inline uint32_t size() const {
    return capacity_bits >> 3;
  }

private:
  void set_databus_mode(const uint8_t inout);

  inline uint8_t read_databus() {
    //return (PINB & PORTB_MASK) | (PIND & PORTD_MASK);
    //return ((PINC & PORTC_MASK)>>4) | (PIND & PORTD_MASK);
    return databus.read();
  }

  inline void write_databus(const uint8_t val) {
    /*
    PORTD &= ~PORTD_MASK;
    PORTD |= val & PORTD_MASK;
    //PORTB &= ~PORTB_MASK;
    //PORTB |= val & PORTB_MASK;
    PORTC &= ~PORTC_MASK;
    PORTB |= (val & (PORTC_MASK>>4)) << 4;
    */
    databus.write(val);
  }

  inline void set_address(const uint32_t& addr) {
    addrbus.write32(addr);
  }

  void put_byte(const uint32_t& addr, const uint8_t data);

  bool waitfor_write_cycle_end(const uint8_t & data);

public:
  uint8_t read(const uint32_t& addr);
  uint8_t write(const uint32_t& addr, const uint8_t data);

  // Write the special six-byte code to turn off Software Data Protection.
  bool disable_SDP();

  bool program_byte(const uint32_t& addr, const uint8_t data);
  bool program_page(const uint32_t& addr, const uint8_t data[], uint16_t page_size);

};


#endif /* MEMORY_H_ */
