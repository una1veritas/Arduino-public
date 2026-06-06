/*
 * pagearray.h
 *
 *  Created on: 2026/05/18
 *      Author: sin
 */

#ifndef PAGEARRAY_H_
#define PAGEARRAY_H_

#include <SPI.h>
#include <SPISRAM.h>

struct PageBuffer {
	static const uint16_t page_size = 64;	// limit in bytes
	static const uint32_t page_mask = page_size - 1;

	uint32_t address;	// absolute starting address
	uint16_t length; 		// the number of byte data
	uint8_t  bytes[page_size];

	// utility
	static uint32_t is_page_boundary(const uint32_t & addr) { return (addr & uint32_t(page_mask)) == 0 ; }

	PageBuffer(const uint32_t & addr, const uint8_t data[], const uint16_t & len) :
		address( addr ),
		length( len > page_size ? page_size : len )
	{
		for(uint16_t i = 0; i < len ; ++i) {
			bytes[i] = data[i];
		}
	}

	PageBuffer(void) : address(0), length(0) {}

	PageBuffer(const uint32_t & addr) : address(addr), length(0) {}

	PageBuffer(const PageBuffer & another) {
		*this = another;
	}

	PageBuffer & operator=(const PageBuffer & another) {
		address = another.address;
		length = another.length;
		for(uint16_t i = 0; i < length; ++i) {
			bytes[i] = another.bytes[i];
		}
		return *this;
	}

	bool is_filled() const {
		return length == page_size;
	}

	bool is_aligned() const {
		return is_page_boundary(address);
	}

	friend bool operator<(const PageBuffer & l, const PageBuffer & r) {
		if ( l.address < r.address ) {
			return true;
		} else if ( l.address == r.address ) {
			return l.length < r.length;
		}
		return false;
	}

	// returns true if append a byte is succeeded
	bool append(const uint8_t val);

	size_t printOn(Stream & out) const;

};


// Variable length memory page block array
struct PageArray {
	SPISRAM auxarray;
	uint32_t start_offset; // offset address to the first page block.
	uint32_t pages_count;


	PageArray(const uint8_t SRAM_CS) :
		auxarray(SPISRAM(SRAM_CS, SPISRAM::BUS_WIDTH_23LC1024)),
		start_offset(0),
		pages_count(0) {
	}

	void begin() {
		auxarray.begin();
	}

	inline uint32_t size() const { return pages_count; }

	void clear() {
		start_offset = 0;
		pages_count = 0;
	}

	uint32_t total_bytes() const;

	// the first address appears in pages
	uint32_t lowest_address() const;

	// the largest value of the last address (= page.address + page.length - 1) among all the pages
	uint32_t highest_address() const;

	void get_byindex(const uint32_t & index, PageBuffer & page) const ;
	void set_byindex(const uint32_t & index, PageBuffer &page) ;

	// create new page block then add to the next of the last, as tail page block.
	uint16_t append_pages(const uint32_t & addr, const uint8_t data[], const uint16_t & length);

	void append_bytes(uint32_t address, const uint8_t * data, uint16_t length);

	size_t printOn(Stream & out);

	void sort_pages();
};



#endif /* PAGEARRAY_H_ */
