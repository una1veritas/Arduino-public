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

struct Page64 {
	static const uint16_t page_size = 64;

	uint32_t address;	// absolute address where data[0] must be aligned and be placed.
	uint16_t length; 		//
	uint8_t  data[page_size];

	Page64(uint32_t & addr, uint8_t data[], uint16_t len) :
		address( addr ),
		length( len )
	{
		uint32_t page_boundary = (address & ~uint32_t(page_size - 1)) + page_size;
		if ( address + len > page_boundary) {
			length = page_boundary - address;
		}
		for(uint16_t i = 0; i < length; ++i) {
			this->data[i] = data[i];
		}
	}

	Page64(void) {}

	Page64(const Page64 & another) :
		address( another.address ),
		length( another.length )
	{
		for(uint16_t i = 0; i < page_size; ++i) {
			data[i] = another.data[i];
		}
	}

	Page64 & copy(const Page64 & another) {
		address = another.address;
		length = another.length;
		for(uint16_t i = 0; i < page_size; ++i) {
			data[i] = another.data[i];
		}
		return *this;
	}

	uint8_t & operator[](const uint16_t & index) {
		return data[index];
	}

	Page64 & operator=(const Page64 & another) {
		copy(another);
		return *this;
	}

	bool is_full() const {
		uint32_t page_boundary = (address & ~uint32_t(page_size - 1)) + page_size;
		return address + length == page_boundary;
	}

	bool is_aligned() const {
		return (address & (page_size - 1)) == 0 ;
	}

	uint16_t remaining_capacity() {
		uint32_t page_boundary = (address & ~uint32_t(page_size - 1)) + page_size;
		return page_boundary - (address + length);
	}

	size_t printOn(Stream & out);

};


// Variable memory block array
struct PageArray {
	SPISRAM auxarray;
	uint32_t head_ix; // indexes of the first page block.
	uint32_t pages_count;


	PageArray(const uint8_t SRAM_CS) :
		auxarray(SPISRAM(SRAM_CS, SPISRAM::BUS_WIDTH_23LC1024)),
		head_ix(0),
		pages_count(0) {
	}

	void begin() {
		auxarray.begin();
	}

	inline uint32_t size() { return pages_count; }

	void clear() {
		head_ix = 0;
		pages_count = 0;
	}

/*	Page64 & load_page(const uint32_t & auxindex, Page64 & page) {
		for(uint32_t i = 0; i < sizeof(Page64); ++i) {
			*(((uint8_t *)&page) + i) = auxarray.read(auxindex + i);
		}
		return page;
	}

	Page64 & store_page(const uint32_t & auxindex, Page64 & page) {
		for(uint32_t i = 0; i < sizeof(Page64); ++i) {
			auxarray.write(auxindex + i, *(((uint8_t *)&page) + i));
		}
		return page;
	}*/

	Page64 & load(const uint32_t & arrayindex, Page64 & page) {
		uint32_t auxindex = head_ix + arrayindex * sizeof(Page64);
		for (uint32_t i = 0; i < sizeof(Page64); ++i) {
			*(((uint8_t*) &page) + i) = auxarray.read(auxindex + i);
		}
		return page;
	}

	void store(const uint32_t & arrayindex, const Page64 &page) {
		uint32_t auxindex = head_ix + arrayindex * sizeof(Page64);
		for (uint32_t i = 0; i < sizeof(Page64); ++i) {
			auxarray.write(auxindex + i, *(((uint8_t*) &page) + i));
		}
		return;
	}

	// create new page block then add to the next of the last, as tail page block.
	uint16_t append_page(const uint32_t & addr, const uint8_t data[], const uint16_t & length);

	void append_bytes(uint32_t address, uint8_t * data, uint16_t length);

	size_t printOn(Stream & out);
};



#endif /* PAGEARRAY_H_ */
