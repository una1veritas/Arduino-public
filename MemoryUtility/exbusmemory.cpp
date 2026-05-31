/*
 * memory.cpp
 *
 *  Created on: 2026/05/16
 *      Author: sin
 */

#include "exbusmemory.h"

void ExBusMemory::set_databus_mode(const uint8_t inout) {
	if (inout == INPUT) {
		databus.enable_gpio_pullup();
		databus.set_gpio_input();
	} else { // OUTPUT
		databus.set_gpio_output();
		databus.disable_gpio_pullup();
	}
}


// basic read sequence with additional 1 clock wait between /OE to read databus
// certified w/ /CE to data valid 150 ns, /OE to data valid 70 ns

// Success: SRAM M68AF127BM55MC6 (55ns)
uint8_t ExBusMemory::read(const uint32_t &addr) const {
	uint8_t val;
	//output_disable();
	//write_disable();
	set_databus_mode(INPUT);
	write_addressbus(addr);
	select();  		// more than 62.5ns
	output_enable(); 	// 62.5ns
	delay1nop(); 	// 62.5ns
	val = read_databus();
	output_disable();
	deselect();
	return val;
}


// slow EEPROM HN58C256 (up to 5MHz ?)
// valid address to output delay <= 200 ns,
// /CE to output delay <= 200 ns,
// /OE to output delay >= 10 ns, <= 90 ns,
// /OE, /CE high to output float <= 70 ns
// OK for HN58C256
uint8_t ExBusMemory::read(const uint32_t & addr, const uint8_t n) const {
	uint8_t val;
  //output_disable();
  //write_disable();
  set_databus_mode(INPUT);
  write_addressbus(addr);
  select();  		// more than 62.5 ns
  delay1nop();
  output_enable(); 	// 62.5 ns
  delay_nops(n);
  val = read_databus();
  output_disable();
  deselect();
  return val;
}

// basic write to SRAM with 1 clock wait after /CE and 1 clock after /WE

// Success: SRAM M68AF127BM55MC6 (55ns)
void ExBusMemory::write(const uint32_t & addr, const uint8_t data) {
  set_databus_mode(OUTPUT);
  //output_disable();
  //write_disable();
  write_addressbus(addr);
  write_databus(data);
  select();
  write_enable();
  delay1nop();
  write_disable();
  deselect();
  set_databus_mode(INPUT);
  return;
}

// write to EEPROM

uint8_t ExBusMemory::get_byte(const uint32_t & addr) const {
  //output_disable();
  //write_disable();
  write_addressbus(addr);
  delay1nop(); 	// 62.5 ns
  output_enable(); 	// 62.5 ns
  delay1nop(); 	// 62.5 ns
  uint8_t val = read_databus();
  output_disable();
  return val;
}

// for eeprom continuous write
/*
void Memory::put_byte(const uint32_t& addr, const uint8_t data) {
  set_address(addr);
  write_databus(data);
  write_enable();
  delay_62ns();
  delay_62ns();
  write_disable();
  delay_62ns();
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
*/
// partial eeprom write sequence after bus mode change and /CE, before CE, with address change
void ExBusMemory::put_byte(const uint32_t& addr, const uint8_t data) {
  write_addressbus(addr);
  write_databus(data);
  delay1nop();
  delay1nop();
  write_enable();
  delay1nop();
  delay1nop();
  write_disable();
  delay1nop();
}

// OK w/ HN58C256
void ExBusMemory::put_byte(const uint32_t& addr, const uint8_t data, const uint8_t n) {
  write_addressbus(addr);
  write_databus(data);
  write_enable();
  delay_nops(n);
  write_disable();
  delay1nop();
}

// Succeeded: AT28C64-15 w/ n == 1
bool ExBusMemory::program_byte(const uint32_t &addr, const uint8_t data) {
	set_databus_mode(OUTPUT);
	//output_disable();
	//write_disable(); 	// to ensure pulse
	select();
	put_byte(addr, data);

	bool succ = waitfor_write_cycle_end(data, 10000);
	deselect();
	set_databus_mode(INPUT);

	return succ;
}

/*
// OK w/ HN58C256
bool ExBusMemory::program_byte_100ns(const uint32_t& addr, const uint8_t data) {
  set_databus_mode(OUTPUT);
  //output_disable();
  write_disable(); 	// to ensure pulse
  write_addressbus(addr);
  write_databus(data);
  select();
  delay1clock();
  delay1clock();
  write_enable();
  delay1clock();
  delay1clock();
  write_disable();

  set_databus_mode(INPUT);
  bool succ = waitfor_write_cycle_end(data, 10000);
  deselect();

  return succ;
}
*/


// write to EEPROM
// OK w/ HN58C256
bool ExBusMemory::program_page(const uint32_t & addr, const uint8_t data[], uint16_t page_size, const uint8_t n) {
	uint8_t val;
  uint32_t baseaddr = (~uint32_t(page_size - 1)) & addr;
  set_databus_mode(OUTPUT);
  select();
  for(uint16_t ix = 0; ix < page_size; ++ix) {
	  val = data[ix];
	  put_byte(baseaddr + ix, val, n);
  }
  set_databus_mode(INPUT);
  bool succ = waitfor_write_cycle_end(val, 10000);
  deselect();
  return succ;
}

// OK w/ HN58C256
bool ExBusMemory::waitfor_write_cycle_end(const uint8_t & data, const uint16_t & count) {
	uint8_t val0, val1;
	//set_databus_mode(INPUT);
	for(uint16_t i = 0; i < count; ++i) {
		select();
	    output_enable();
	    delay1nop();
	    val0 = read_databus();
	    output_disable();
	    deselect();
	    delay_nops(1);
	    select();
	    output_enable();
	    delay1nop();
	    val1 = read_databus();
	    output_disable();
	    if ( val0 == val1 and val0 == data )
	      break;
	  }
	  return val0 == data;
}


// Write the special six-byte code to turn off Software Data Protection.
bool ExBusMemory::disable_SDP() {
    //disableOutput();
    //disableWrite();
    set_databus_mode(OUTPUT);
    select();

    put_byte(0x5555, 0xaa);
    put_byte(0x2aaa, 0x55);
    put_byte(0x5555, 0x80);
    put_byte(0x5555, 0xaa);
    put_byte(0x2aaa, 0x55);
    put_byte(0x5555, 0x20);

    deselect();
    set_databus_mode(INPUT);

    return true;
}

// Write the special three-byte code to turn on Software Data Protection.
bool ExBusMemory::enable_SDP() {
    //disableOutput();
    //disableWrite();
    set_databus_mode(OUTPUT);
    select();

    put_byte(0x5555, 0xaa);
    put_byte(0x2aaa, 0x55);
    put_byte(0x5555, 0xa0);

    deselect();
    set_databus_mode(INPUT);

    return true;
}

void ExBusMemory::down_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t val) {
	for(uint32_t ix = 0; ix < block_size; ++ix) {
		write(base_addr + ix, val);
	}
}

void ExBusMemory::up_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t val) {
	for(uint32_t ix = block_size; ix > 0; ) {
		--ix;
		write(base_addr + ix, val);
	}
}

uint32_t ExBusMemory::down_read_verify_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t vval, uint8_t wval) {
	uint32_t errcount = 0;
	select();
	for(uint32_t ix = 0; ix < block_size; ++ix) {
	    set_databus_mode(INPUT);
		uint8_t val = get_byte(base_addr + ix);
		if ( val != vval )
			++errcount;
		// write without set address bus
	    set_databus_mode(OUTPUT);
	    write_enable();
	    delay1nop();
	    write_databus(wval);
	    delay1nop();
	    write_disable();
	    delay1nop();
	}
    set_databus_mode(INPUT);
	deselect();
	return errcount;
}

uint32_t ExBusMemory::up_read_verify_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t vval, uint8_t wval) {
	uint32_t errcount = 0;
	select();
	for(uint32_t ix = block_size; ix > 0; ) {
	    set_databus_mode(INPUT);
		--ix;
		uint8_t val = get_byte(base_addr + ix);
		if ( val != vval )
			++errcount;
		// write without set address bus
	    set_databus_mode(OUTPUT);
	    write_enable();
	    delay1nop();
	    write_databus(wval);
	    delay1nop();
	    write_disable();
	    delay1nop();
	}
    set_databus_mode(INPUT);
	deselect();
	return errcount;
}

// March C- 8bit
uint32_t ExBusMemory::sram_check(const uint32_t & start, const uint32_t & block_size) {
	uint32_t errcount = 0 ;
	uint32_t addr, ix;
	uint8_t val;

	errcount = 0;
	// ⇕0(w00000000);
	down_write(addr, block_size, 0);
	up_write(addr, block_size, 0);

	//⇑1(r00000000,w11111111);
	errcount += up_read_verify_write(addr, block_size, 0, 0xff);

	// ⇑2(r11111111,w00000000);
	errcount += up_read_verify_write(addr, block_size, 0xff, 0);

	// ⇓3(r00000000,w11111111);
	errcount += down_read_verify_write(addr, block_size, 0, 0xff);

	//⇓4(r11111111,w00000000);
	errcount += down_read_verify_write(addr, block_size, 0xff, 0);

	// ⇓5(r00000000, w01010101); ⇑6 (r01010101, w10101010);
	errcount += down_read_verify_write(addr, block_size, 0, 0x55);
	errcount += up_read_verify_write(addr, block_size, 0x55, 0xaa);

	// ⇓7(r10101010, w01010101); ⇑8(r01010101, w00110011);
	errcount += down_read_verify_write(addr, block_size, 0xaa, 0x55);
	errcount += up_read_verify_write(addr, block_size, 0x55, 0x33);

	// ⇓9(r00110011, w11001100); ⇑10(r11001100, w00110011);
	errcount += down_read_verify_write(addr, block_size, 0x33, 0xcc);
	errcount += up_read_verify_write(addr, block_size, 0xcc, 0x33);

	// ⇓11(r00110011, w00001111); ⇑12(r00001111, w11110000);
	errcount += down_read_verify_write(addr, block_size, 0x33, 0x0f);
	errcount += up_read_verify_write(addr, block_size, 0x0f, 0xf0);

	// ⇓13(r11110000, w00001111); ⇑14(r00001111)}
	errcount += down_read_verify_write(addr, block_size, 0xf0, 0x0f);
	errcount += up_read_verify_write(addr, block_size, 0x0f, 0);

	return errcount;
}
