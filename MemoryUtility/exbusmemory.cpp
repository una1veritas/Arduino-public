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
	// output_disable();
	// write_disable();
	// set_databus_mode(INPUT);

	write_addressbus(addr);
	select();
	output_enable();
	delay1nop();
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



// slow EEPROM HN58C256 (up to 5MHz ?)
// valid address to output delay <= 200 ns,
// /CE to output delay <= 200 ns,
// /OE to output delay >= 10 ns, <= 90 ns,
// /OE, /CE high to output float <= 70 ns

// T_ACC Address to output delay > (select + 2 nops + output enable + 2 nops)
// T_CE  /CE to output dealy     >          (2 nops + output enable + 2 nops)
// T_OE  /OE to output delay > ( 2 nops )

// succeeded: AT28C64/B, HN58C256, AT29C256 (flash)

uint8_t ExBusMemory::read_rom(const uint32_t & addr) const {
	uint8_t val;
  //output_disable();
  //write_disable();
  // set_databus_mode(INPUT);

  write_addressbus(addr);
  select();
  output_enable();
  delay2nops();
  val = read_databus();
  output_disable();
  deselect();

  return val;
}


// Succeeded: AT28C64-15, HN58C256
bool ExBusMemory::program_byte(const uint32_t &addr, const uint8_t data) {
	//output_disable();	// make sure
	//write_disable(); 	// to ensure pulse
	set_databus_mode(OUTPUT);

	write_addressbus(addr);
	write_databus(data);

	select();

	write_enable();
	delay2nops(); 		// >= 100 ns
	write_disable();
	delay1nop();

	set_databus_mode(INPUT);

	bool succ = waitfor_write_cycle_end(data, 4000);
	deselect();

	return succ;
}


// write to EEPROM
// Succeeded: AT28C64B, HN58C256, AT29C256 (flash)
bool ExBusMemory::program_page(const uint32_t &addr, const uint8_t data[], uint16_t page_size) {
	uint8_t val;
	uint32_t baseaddr = (~uint32_t(page_size - 1)) & addr; 	// to ensure and to force page boundary

	set_databus_mode(OUTPUT);
	output_disable();	// make sure
	write_disable(); 	// to ensure pulse

	select();

	for (uint16_t ix = 0; ix < page_size; ++ix) {
		val = data[ix];
		//put_byte(baseaddr + ix, data[ix]);
		write_addressbus(baseaddr + ix);
		write_databus(val);

		write_enable();
		delay2nops();
		write_disable();
		delay1nop();
	}
	set_databus_mode(INPUT);

	bool succ = waitfor_write_cycle_end(val, 4000);
	deselect();

	return succ;
}

// in continuous read from EEPROM
uint8_t ExBusMemory::get_byte(const uint32_t & addr) const {
  //output_disable();
  //write_disable();
  write_addressbus(addr);
  delay2nops(); 	// 62.5 ns x 2
  output_enable();
  delay2nops(); 	// 62.5 ns x 2
  uint8_t val = read_databus();
  output_disable();
  return val;
}

// for eeprom continuous write
// T_AH address hold while /WE low > (data bus I/O + 2 nops) >= min 150ns
// T_WP /WE pulse width            > ( 2 nops ) >= min 150ns
// T_DH data setup before capture > >= 100ns
// T_BLC byte load cycle 0.35 -- 30 us
// T_BL byte load window (address change) > min 100us
// T_DL data latch time (/WE is high) > 200ns

void ExBusMemory::put_byte(const uint32_t &addr, const uint8_t data) {
	write_addressbus(addr);
	write_databus(data);

	write_enable();
	delay2nops();
	write_disable();
	delay1nop();
}

// HN58C256, AT28C64B,  AT29C256 (flash)
bool ExBusMemory::waitfor_write_cycle_end(const uint8_t & data, const uint16_t & count) {
	uint8_t val0, val1;
	uint16_t i = 0;

	do {
		delayMicroseconds(1);

	    output_enable();
	    delay2nops();
	    val0 = read_databus();
	    output_disable();

	    delayMicroseconds(1);

	    output_enable();
	    delay2nops();
	    val1 = read_databus();
	    output_disable();

	    if ( val0 == val1 and val1 == data ) {
	    	// toggle bit 6 and data poll bit 7 are settled
	    	return true;
	    }
	    // 30 us + 31 us
	} while ( i++ < count );
	//Serial.println(i);  // loops 126 at page write, 125 at byte write in successful write
	 return false;
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
	write_disable();
	output_disable();
	select();
	for(uint32_t ix = 0; ix < block_size; ++ix) {
	    set_databus_mode(INPUT);
		//uint8_t val = get_byte(base_addr + ix);
	    write_addressbus(base_addr + ix);
	    delay1nop(); 	// 62.5 ns
	    output_enable();
	    delay1nop(); 	// 62.5 ns
	    uint8_t val = read_databus();
	    output_disable();

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
	deselect();
    set_databus_mode(INPUT);
	return errcount;
}

uint32_t ExBusMemory::up_read_verify_write(const uint32_t & base_addr, const uint32_t block_size, uint8_t vval, uint8_t wval) {
	uint32_t errcount = 0;
	write_disable();
	output_disable();
	select();
	for(uint32_t ix = block_size; ix > 0; ) {
	    set_databus_mode(INPUT);
		--ix;
		//uint8_t val = get_byte(base_addr + ix);
	    write_addressbus(base_addr + ix);
	    delay1nop(); 	// 62.5 ns
	    output_enable();
	    delay1nop(); 	// 62.5 ns
	    uint8_t val = read_databus();
	    output_disable();

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
