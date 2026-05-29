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
		// ⇕0(w00000000);
		for(ix = 0; ix < block_size; ++ix) {
			mem.write(addr+ix, 0);
		}
		for(ix = 0; ix < block_size; ++ix) {
			mem.write(addr + block_size - 1 - ix, 0);
		}
		//⇑1(r00000000,w11111111);
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0 ) {
				errcount++;
			}
			mem.write(addr + ix, 0xff);
		}
		// ⇑2(r11111111,w00000000);
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0xff ) {
				errcount++;
			}
			mem.write(addr + ix, 0);
		}

		// ⇓3(r00000000,w11111111);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0 ) {
				errcount++;
			}
			mem.write(addr + ix, 0xff);
		}
		//⇓4(r11111111,w00000000);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0xff ) {
				errcount++;
			}
			mem.write(addr + ix, 0);
		}

		// ⇓5(r00000000, w01010101); ⇑6 (r01010101, w10101010);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0 ) {
				errcount++;
			}
			mem.write(addr + ix, 0x55);
		}
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0x55 ) {
				errcount++;
			}
			mem.write(addr + ix, 0xaa);
		}

		// ⇓7(r10101010, w01010101); ⇑8(r01010101, w00110011);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0xaa ) {
				errcount++;
			}
			mem.write(addr + ix, 0x55);
		}
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0x55 ) {
				errcount++;
			}
			mem.write(addr + ix, 0x33);
		}

		// ⇓9(r00110011, w11001100); ⇑10(r11001100, w00110011);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0x33 ) {
				errcount++;
			}
			mem.write(addr + ix, 0xcc);
		}
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0xcc ) {
				errcount++;
			}
			mem.write(addr + ix, 0x33);
		}

		// ⇓11(r00110011, w00001111); ⇑12(r00001111, w11110000);
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0x33 ) {
				errcount++;
			}
			mem.write(addr + ix, 0x0f);
		}
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0x0f ) {
				errcount++;
			}
			mem.write(addr + ix, 0xf0);
		}

		// ⇓13(r11110000, w00001111); ⇑14(r00001111)}
		for(ix = 0; ix < block_size; ++ix) {
			val = mem.read(addr+ix);
			if ( val != 0xf0 ) {
				errcount++;
			}
			mem.write(addr + ix, 0x0f);
		}
		for(ix = block_size; ix > 0; ) {
			--ix;
			val = mem.read(addr + ix);
			if ( val != 0x0f ) {
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
