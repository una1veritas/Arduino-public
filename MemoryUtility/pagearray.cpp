/*
 * pagearray.cpp
 *
 *  Created on: 2026/05/18
 *      Author: sin
 */

#include "pagearray.h"


size_t Page64::printOn(Stream &out) const {
	char tmp16[16];
	size_t count = 0;
	for (uint16_t i = 0; i < length; ++i) {
		if (i == 0) {
			snprintf(tmp16, 15, "%04lX: ", address + i);
			count += out.print(tmp16);
		} else if ( i % 16 == 0 ) {
			count += out.println();
			count += out.print(F("      ") );
		}
		snprintf(tmp16, 15, "%02X ", data[i]);
		count += out.print(tmp16);
	}
	count += out.println();
	return count;
}

void PageArray::load(Page64 & page, const uint32_t & index) const {
	uint32_t auxaddr = start_offset + index * sizeof(Page64);
	auxarray.read(auxaddr, (uint8_t*) & page , sizeof(Page64));
	return;
}

void PageArray::store(const uint32_t & index, Page64 & page) {
	uint32_t auxaddr = start_offset + index * sizeof(Page64);
	auxarray.write(auxaddr, (uint8_t*) & page, sizeof(Page64));
	return;
}

uint32_t PageArray::total_bytes() const {
	Page64 page;
	uint32_t total = 0;
	for(uint16_t ix = 0; ix < pages_count; ++ix) {
		load(page, ix);
		total += page.length;
	}
	return total;
}

uint32_t PageArray::lowest_address() const {
	Page64 page;
	uint32_t addr = 0;
	if ( pages_count == 0 )
		return addr;
	load(page, 0);
	addr = page.address;
	for(uint16_t ix = 1; ix < pages_count; ++ix) {
		load(page, ix);
		if ( page.address < addr )
			addr = page.address;
	}
	return addr;
}

uint32_t PageArray::end_address() const {
	uint32_t addr = 0;
	if ( pages_count == 0 )
		return addr;
	Page64 page;
	load(page, 0);
	addr = page.address + page.length;
	for(uint16_t ix = 1; ix < pages_count; ++ix) {
		load(page, ix);
		if ( page.address + page.length > addr )
			addr = page.address + page.length;
	}
	return addr;
}

	// create new page block then add to the next of the last, as tail page block.
uint16_t PageArray::append_page(const uint32_t &addr, const uint8_t data[],
		const uint16_t &length) {
	Page64 pagecache;

	pagecache.address = addr;
	uint32_t page_boundary = (addr & (~uint32_t(pagecache.page_size - 1)))
			+ pagecache.page_size;

	pagecache.length =
			length > page_boundary - addr ? page_boundary - addr : length;
	for (uint16_t i = 0; i < pagecache.length; ++i) {
		pagecache[i] = data[i];
	}
	store(pages_count, pagecache); //store_page(pages_count * sizeof(Page64), pagecache);
	pages_count += 1;
	return pagecache.length;
}

void PageArray::append_bytes(uint32_t address, const uint8_t *data, uint16_t length) {
	Page64 lastpage;
	load(lastpage, pages_count - 1); //load_page(head_ix + sizeof(Page64) * (pages_count - 1), lastpage);

	uint16_t ix = 0; // offset index to data
	// update to fill up the last page
	if ( ! lastpage.is_filled() and address == lastpage.address + lastpage.length) {
		uint16_t remain = lastpage.remaining_capacity();
		for (; ix < remain and ix < length; ++ix) {
			lastpage[lastpage.length + ix] = data[ix];
		}
		lastpage.length += ix;
		store(pages_count - 1, lastpage); //store_page((pages_count - 1) * sizeof(Page64), lastpage); // now the last page block is full (to the page boundary)
	}
	// append new pages
	for (; ix < length; ix += lastpage.page_size) {
		append_page(address + ix, data + ix,
				length - ix);
	}

}

size_t PageArray::printOn(Stream &out) {
	Page64 page;
	size_t count = 0;
	for (uint16_t i = 0; i < pages_count; ++i) {
		load(page, i);
		count += page.printOn(Serial);
		count += Serial.println();
	}
	return count;
}
