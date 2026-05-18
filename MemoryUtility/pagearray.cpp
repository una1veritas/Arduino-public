/*
 * pagearray.cpp
 *
 *  Created on: 2026/05/18
 *      Author: sin
 */

#include "pagearray.h"


size_t Page64::printOn(Stream &out) {
	char tmp16[16];
	size_t count = 0;
	for (int i = 0; i < length; ++i) {
		if (i == 0) {
			snprintf(tmp16, 15, "%04X : ", address + i);
			count += out.print(tmp16);
		} else if ( i % 16 == 0 ) {
			count += out.println();
			count += out.print(F("       ") );
		}
		snprintf(tmp16, 15, "%02X ", data[i]);
		count += out.print(tmp16);
	}
	count += out.println();
	return count;
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

void PageArray::append_bytes(uint32_t address, uint8_t *data, uint16_t length) {
	Page64 lastpage;
	load(pages_count - 1, lastpage); //load_page(head_ix + sizeof(Page64) * (pages_count - 1), lastpage);

	uint16_t ix = 0; // offset index to data
	// update to fill up the last page
	if (!lastpage.is_full() and address == lastpage.address + lastpage.length) {
		uint16_t remain = lastpage.remaining_capacity();
		for (; ix < remain and ix < length; ++ix) {
			lastpage[lastpage.length + ix] = data[ix];
		}
		lastpage.length += ix;
		store(pages_count - 1, lastpage); //store_page((pages_count - 1) * sizeof(Page64), lastpage); // now the last page block is full (to the page boundary)
	}
	// append new pages
	for (; ix < length; ix += lastpage.page_size) {
		uint16_t bytes_stored = append_page(address + ix, data + ix,
				length - ix);
	}

}

size_t PageArray::printOn(Stream &out) {
	Page64 page;
	size_t count = 0;
	for (uint16_t i = 0; i < pages_count; ++i) {
		load(i, page);
		count += page.printOn(Serial);
		count += Serial.println();
	}
	return count;
}
