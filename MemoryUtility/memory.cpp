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
}

// basic read sequence with additional 1 clock wait between /OE to read databus
uint8_t Memory::read(const uint32_t & addr) {
  //output_disable();
  //write_disable();
  set_databus_mode(INPUT);
  set_address(addr);
  select();  		// more than 62.5ns
  output_enable(); 	// 62.5ns
  delay1clock(); 	// wait duration + calling read_databus over head duration must be larger than access time
  uint8_t val = read_databus();
  output_disable();
  deselect();
  return val;
}

// basic write to SRA< with 1 clock wait after /CE and 1 clock after /WE
uint8_t Memory::write(const uint32_t & addr, const uint8_t data) {
  set_databus_mode(OUTPUT);
  output_disable();
  //write_disable();
  set_address(addr);
  write_databus(data);
  select();
  delay1clock();
  write_enable();
  delay1clock();
  write_disable();
  deselect();
  return data;
}

// partial eeprom write sequence after bus mode change and /CE, before CE, with address change
void Memory::put_byte(const uint32_t& addr, const uint8_t data) {
  set_address(addr);
  write_enable();
  write_databus(data);
  delay1clock();
  write_disable();
  delay1clock();
}

// write to EEPROM
// succeeded with at28c64

bool Memory::program_byte(const uint32_t& addr, const uint8_t data) {
  set_databus_mode(OUTPUT);
  output_disable();
  write_disable(); 	// to ensure pulse
  set_address(addr);
  select();
  //delay1clock();
  write_enable();
  write_databus(data);
  delay1clock();
  write_disable();

  bool succ = waitfor_write_cycle_end(data, 1000);
  deselect();
  set_databus_mode(INPUT);

  return succ;
}

// write to EEPROM
bool Memory::program_page(const uint32_t & addr, const uint8_t data[], uint16_t page_size) {
	uint8_t val;
  uint32_t baseaddr = (~uint32_t(page_size - 1)) & addr;
  set_databus_mode(OUTPUT);
  select();
  for(uint16_t ix = 0; ix < page_size; ++ix) {
	  val = data[ix];
	  put_byte(baseaddr + ix, val);
  }
  bool succ = waitfor_write_cycle_end(val, 1000);
  deselect();
  return succ;
}

bool Memory::waitfor_write_cycle_end(const uint8_t & data, uint16_t count) {
	uint8_t val0, val1;
	set_databus_mode(INPUT);
	do {
		select();
	    output_enable();
	    delay1clock();
	    val0 = read_databus();
	    output_disable();
	    deselect();
	    select();
	    output_enable();
	    delay1clock();
	    val1 = read_databus();
	    output_disable();
	    if ( !(count-- > 0) ) {
	      return false;
	    }
	  } while ( val0 != val1 );
	  return val0 == data;
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

// Write the special three-byte code to turn on Software Data Protection.
bool Memory::enable_SDP() {
    //disableOutput();
    //disableWrite();
    select();
    set_databus_mode(OUTPUT);

    put_byte(0x5555, 0xaa);
    put_byte(0x2aaa, 0x55);
    put_byte(0x5555, 0xa0);

    set_databus_mode(INPUT);
    deselect();

    return true;
}

