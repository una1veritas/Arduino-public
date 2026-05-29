//============================================================================
// Name        : SRAM_check_algorithm.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

struct MEMORY {
	uint8_t bytes[32*1024];

	void write(const uint32_t & addr, uint8_t data) {
		bytes[addr] = data;
	}

	uint8_t read(const uint32_t & addr) const {
		return bytes[addr];
	}
} mem;

struct ROMINFO {
	uint8_t type_id;
	uint32_t capacity_inbits;
    uint16_t page_size;
    char name[16];
};

void March_Cminus(ROMINFO & meminfo) {
	uint32_t errcount = 0 ;
	uint32_t addr, ix;
	uint8_t val;
	const uint32_t capacity_inbytes = meminfo.capacity_inbits >> 3 ;
	const uint32_t block_size = capacity_inbytes > 0x800 ? 0x800 : capacity_inbytes ;
	for(addr = 0; addr < capacity_inbytes; addr += block_size) {
		errcount = 0;
		printf("%04X -- %04X: ", addr, addr + block_size - 1);
		// ⇕(w0);
		for(ix = 0; ix < block_size; ++ix) {
			mem.write(addr+ix, 0);
		}
		for(ix = 0; ix < block_size; ++ix) {
			mem.write(addr + block_size - 1 - ix, 0);
		}
		// ⇑(r0,w1);
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0 ) {
				errcount++;
			}
			mem.write(addr + ix, 1);
		}
		// ⇑(r1,w0);
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 1 ) {
				errcount++;
			}
			mem.write(addr + ix, 0);
		}
		// ⇓(r0,w1);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0 ) {
				errcount++;
			}
			mem.write(addr + ix, 1);
		}
		// ⇓(r1,w0);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 1 ) {
				errcount++;
			}
			mem.write(addr + ix, 0);
		}
		// ⇕(r0)
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0 ) {
				errcount++;
			}
		}
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr+ix);
			if ( val != 0 ) {
				errcount++;
			}
		}
		printf("%d\n", errcount);
	}
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	ROMINFO meminfo = { 0, 32 * 8 * 1024UL, 64, "HN58C256" };
	March_Cminus(meminfo);
	return 0;
}
