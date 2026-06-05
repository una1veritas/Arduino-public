/*
 * pagearray.cpp
 *
 *  Created on: 2026/05/18
 *      Author: sin
 */

#include "pagearray.h"

bool Page64::append(const uint8_t val) {
	if ( is_filled() ) return false;
	data[start + length] = val;
	++length;
	return true;
}

size_t Page64::printOn(Stream &out) const {
	char tmp16[16];
	size_t count = 0;
	for (uint16_t i = 0; i < page_size; ++i) {
		if (i == 0) {
			snprintf(tmp16, 15, "%04lX: ", page_address + i);
			count += out.print(tmp16);
		} else if ( i % 16 == 0 ) {
			count += out.println();
			count += out.print(F("      ") );
		}
		if ( start <= i and i < start + length ) {
			snprintf(tmp16, 15, "%02X ", data[i]);
			count += out.print(tmp16);
		} else {
			count += out.print("   ");
		}
	}
	count += out.println();
	return count;
}

// read out from
void PageArray::get_byindex(const uint32_t & index, Page64 & page) const {
	uint32_t auxaddr = start_offset + index * sizeof(Page64);
	auxarray.read(auxaddr, (uint8_t*) & page , sizeof(Page64));
	return;
}

// write into
void PageArray::set_byindex(const uint32_t & index, Page64 & page) {
	uint32_t auxaddr = start_offset + index * sizeof(Page64);
	auxarray.write(auxaddr, (uint8_t*) & page, sizeof(Page64));
	return;
}

uint32_t PageArray::total_bytes() const {
	Page64 page;
	uint32_t total = 0;
	for(uint16_t ix = 0; ix < pages_count; ++ix) {
		get_byindex(ix, page);
		total += page.length;
	}
	return total;
}

uint32_t PageArray::lowest_address() const {
	Page64 page;
	uint32_t addr = 0;
	if ( pages_count == 0 )
		return addr;
	get_byindex(0, page);
	addr = page.start_address();
	for(uint16_t ix = 1; ix < pages_count; ++ix) {
		get_byindex(ix, page);
		if ( page.start_address() < addr )
			addr = page.start_address();
	}
	return addr;
}

uint32_t PageArray::end_address() const {
	uint32_t addr = 0;
	if ( pages_count == 0 )
		return addr;
	Page64 page;
	get_byindex(0, page);
	addr = page.end_address();
	for(uint16_t ix = 1; ix < pages_count; ++ix) {
		get_byindex(ix, page);
		if ( page.end_address() > addr )
			addr = page.end_address();
	}
	return addr;
}

	// create new page block(s) and add it/them as the last page block(s).
uint16_t PageArray::append_page(const uint32_t &addr, const uint8_t data[],
		const uint16_t &length) {

	uint16_t offset = 0;

	for (uint32_t page_start = addr; page_start < addr + length; ) {
		Page64 pagecache(page_start, data + offset, length);
		set_byindex(pages_count, pagecache); //store_page(pages_count * sizeof(Page64), pagecache);
		pages_count += 1;

		page_start += pagecache.size();
		offset += pagecache.length;
	}
	return offset;
}

void PageArray::append_bytes(uint32_t address, const uint8_t *data, uint16_t length) {
	Page64 lastpage;
	get_byindex(pages_count - 1, lastpage); //load_page(head_ix + sizeof(Page64) * (pages_count - 1), lastpage);

	uint16_t ix = 0; // offset index to data
	// fill up the last page
	if ( ! lastpage.is_filled() and lastpage.end_address() == address ) {
		for (; lastpage.append(data[ix]) and ix < length; ++ix) { }
		set_byindex(pages_count - 1, lastpage); //store_page((pages_count - 1) * sizeof(Page64), lastpage); // now the last page block is full (to the page boundary)
	}
	// append new pages
	for (; ix < length; ix += Page64::page_size) {
		append_page(address + ix, data + ix,
				length - ix);
	}

}

size_t PageArray::printOn(Stream &out) {
	Page64 page;
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
	Page64 a, b; 	// temporaries
    for (uint32_t gap = pages_count >> 1; gap > 0; gap >>= 1) {
        // Perform "gapped" swaps, as far as the chain grow longer
        for (uint32_t i = gap; i < pages_count; i++) {
        	get_byindex(i, a);
        	uint32_t j;
            for (j = i; j >= gap; j -= gap ) {
            	get_byindex(j - gap, b);
            	if ( (a.start_address() < b.start_address() ) or
            			( (a.start_address() == b.start_address() ) and a.end_address() < b.end_address() )) {
            		set_byindex(j, b);
            	} else {
            		break;
            	}
            }
            set_byindex(j, a);
        }
    }
}
