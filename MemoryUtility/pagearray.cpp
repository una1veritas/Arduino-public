/*
 * pagearray.cpp
 *
 *  Created on: 2026/05/18
 *      Author: sin
 */

#include "pagearray.h"

// returns true if success else returns false
bool PageBuffer::append(const uint8_t val) {
	if ( length < page_size ) {
		bytes[length++] = val;
		return true;
	}
	return false; 	// no more space
}

size_t PageBuffer::printOn(Stream &out) const {
	char tmp16[16];
	size_t count = 0;
	uint32_t addr = address & ~uint32_t(0x0f);

	snprintf(tmp16, 15, "%04lX ", addr);
	count += out.print(tmp16);

	for ( ; addr < address + length; ++addr) {
		if ( addr > address and (addr & 0x0f) == 0 ) {
			count += out.println();
			count += out.print(F("     ") );
		}
		if ( address <= addr and addr < address + length ) {
			snprintf(tmp16, 15, "%02X ", bytes[uint16_t(addr - address)]);
			count += out.print(tmp16);
		} else {
			count += out.print("   ");
		}
	}
	count += out.println();
	return count;
}

// read out from
void PageArray::get_byindex(const uint32_t & index, PageBuffer & page) const {
	uint32_t auxaddr = start_offset + index * sizeof(PageBuffer);
	auxarray.read(auxaddr, (uint8_t*) & page , sizeof(PageBuffer));
	return;
}

// write into
void PageArray::set_byindex(const uint32_t & index, PageBuffer & page) {
	uint32_t auxaddr = start_offset + index * sizeof(PageBuffer);
	auxarray.write(auxaddr, (uint8_t*) & page, sizeof(PageBuffer));
	return;
}

uint32_t PageArray::total_bytes() const {
	PageBuffer page;
	uint32_t total = 0;
	for(uint16_t ix = 0; ix < pages_count; ++ix) {
		get_byindex(ix, page);
		total += page.length;
	}
	return total;
}

uint32_t PageArray::lowest_address() const {
	PageBuffer page;
	uint32_t addr = 0;
	if ( pages_count == 0 )
		return addr;
	get_byindex(0, page);
	addr = page.address;
	for(uint16_t ix = 1; ix < pages_count; ++ix) {
		get_byindex(ix, page);
		if ( page.address < addr )
			addr = page.address;
	}
	return addr;
}

uint32_t PageArray::highest_address() const {
	uint32_t addr = 0;
	if ( pages_count == 0 )
		return addr;
	PageBuffer page;
	get_byindex(0, page);
	addr = page.address + page.length - 1;
	for(uint16_t ix = 1; ix < pages_count; ++ix) {
		get_byindex(ix, page);
		if ( page.address + page.length - 1 > addr )
			addr = page.address + page.length - 1;
	}
	return addr;
}

	// create new page block(s) and add it/them as the last page block(s).
uint16_t PageArray::append_pages(const uint32_t &addr, const uint8_t data[], const uint16_t &length) {

	uint16_t offset = 0;
	while ( offset < length ) {
		PageBuffer pcache(addr + offset, data + offset,
				length - offset > PageBuffer::page_size ? PageBuffer::page_size : length - offset);
		set_byindex(pages_count, pcache);
		pages_count += 1;

		offset += pcache.length;
	}
	return offset; 	// the number of bytes
}

void PageArray::append_bytes(uint32_t address, const uint8_t data[], uint16_t length) {
	PageBuffer lastpage;
	get_byindex(pages_count - 1, lastpage);

	uint16_t ix = 0; // offset index to data
	if ( ! lastpage.is_filled() and address == lastpage.address + lastpage.length and ! PageBuffer::is_page_boundary(address) ) {
		//Serial.println("filling.");
		for (; ix < length; ++ix) {
			if ( lastpage.append(data[ix]) == false )
				break;
		}
		set_byindex(pages_count - 1, lastpage);
	}
	// append new pages if needed.
	for (; ix < length; ix += PageBuffer::page_size) {
		append_pages(address + ix, data + ix, length - ix);
	}

}

size_t PageArray::printOn(Stream &out) {
	PageBuffer page;
	size_t count = 0;
	for (uint16_t i = 0; i < pages_count; ++i) {
		get_byindex(i, page);
		count += page.printOn(Serial);
		count += Serial.println();
	}
	return count;
}

// sort pages in ascending order of their starting address in ascending by Shell-sort
void PageArray::sort_pages() {
	// Start with a large gap, then reduce it step by step
	PageBuffer a, b; 	// temporaries
    for (uint32_t gap = pages_count >> 1; gap > 0; gap >>= 1) {
        // Perform "gapped" swaps, as far as the chain grow longer
        for (uint32_t i = gap; i < pages_count; i++) {
        	get_byindex(i, a);
        	uint32_t j;
            for (j = i; j >= gap; j -= gap ) {
            	get_byindex(j - gap, b);
            	if ( a < b ) {
            		set_byindex(j, b);
            	} else {
            		break;
            	}
            }
            set_byindex(j, a);
        }
    }
}
