/*
 * memory.cpp
 *
 *  Created on: 2026/05/16
 *      Author: sin
 */

#include "memory.h"

void Memory::set_databus_mode(const uint8_t inout) {
	if (inout == INPUT) {
		databus.enable_gpio_pullup();
		databus.set_gpio_input();
	} else { // OUTPUT
		databus.set_gpio_output();
		databus.disable_gpio_pullup();
	}
	/*
	 if (inout == INPUT) {
	 //PORTB |= PORTB_MASK;  // set 1 -> weak pull-up
	 PORTC |= PORTC_MASK;
	 PORTD |= PORTD_MASK; // pull-up
	 //DDRB  &= ~PORTB_MASK;  // set 0 -> input
	 DDRC  &= ~PORTC_MASK;
	 DDRD  &= ~PORTD_MASK;  //~0xfc;  // set 0
	 } else if (inout == OUTPUT) {
	 //DDRB |= PORTB_MASK;  // set 1 -> output
	 DDRC |= PORTC_MASK;
	 DDRD |= PORTD_MASK;  // set 1
	 }
	 */
}

uint8_t Memory::read(const uint32_t & addr) {
  set_databus_mode(INPUT);
  //write_disable();
  //output_disable();
  set_address(addr);
  select();
  output_enable();
  delay_62ns();
  uint8_t val = read_databus();
  output_disable();
  deselect();
  return val;
}

uint8_t Memory::write(const uint32_t & addr, const uint8_t data) {
  set_databus_mode(OUTPUT);
  //output_disable();
  //write_disable();
  set_address(addr);
  write_databus(data);
  select();
  delay_62ns();
  write_enable();
  delay_62ns();
  write_disable();
  deselect();
  return data;
}

// for eeprom
void Memory::put_byte(const uint32_t& addr, const uint8_t data) {
  set_address(addr);
  write_databus(data);
  write_enable();
  delay_62ns();
  delay_62ns();
  write_disable();
  delay_62ns();
}

// Write the special six-byte code to turn off Software Data Protection.
bool Memory::disable_SDP() {
    //disableOutput();
    //disableWrite();
    select();
    set_databus_mode(OUTPUT);

    put_byte(0x5555, 0xaa);
    put_byte(0x2aaa, 0x55);
    put_byte(0x5555, 0x80);
    put_byte(0x5555, 0xaa);
    put_byte(0x2aaa, 0x55);
    put_byte(0x5555, 0x20);

    set_databus_mode(INPUT);
    deselect();

    return true;
}

bool Memory::program_byte(const uint32_t& addr, const uint8_t data) {
  set_databus_mode(OUTPUT);
  //output_disable();
  //write_disable();
  select();
  put_byte(addr, data);

  bool succ = waitfor_write_cycle_end(data);
  deselect();
  set_databus_mode(INPUT);

  return succ;
}

bool Memory::program_page(const uint32_t & addr, const uint8_t data[], uint16_t page_size) {
	uint8_t val;
  uint32_t baseaddr = (~uint32_t(page_size - 1)) & addr;
  set_databus_mode(OUTPUT);
  select();
  for(uint16_t ix = 0; ix < page_size; ++ix) {
	  val = data[ix];
	  put_byte(baseaddr + ix, val);
  }
  bool succ = waitfor_write_cycle_end(val);
  deselect();
  return succ;
}

bool Memory::waitfor_write_cycle_end(const uint8_t & data) {
	  set_databus_mode(INPUT);
	  uint32_t counter = 10000;
	  uint8_t val0, val1;
	  do {
	    output_enable();
	    delay_62ns();
	    val0 = read_databus();
	    output_disable();
	    delayMicroseconds(1); //delay_62ns();
	    output_enable();
	    delay_62ns();
	    val1 = read_databus();
	    output_disable();
	    if ( !(counter-- > 0) ) {
	      return false;
	    }
	  } while (val0 != val1 and val1 != data);
	  return true;
}
