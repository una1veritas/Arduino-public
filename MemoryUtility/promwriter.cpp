#include "promwriter.h"

void get_meminfo_byindex(const uint8_t ix, MemoryInfo & dst) {
	memcpy_P(&dst, &MEMINFO_DB[ix], sizeof(MemoryInfo));
}

void get_meminfo_byname(const char name[], MemoryInfo & dst) {
	uint8_t best = 0;
	uint8_t best_ix = 0;
	for(uint8_t ix = 0; ; ++ix) {
		memcpy_P(&dst, &MEMINFO_DB[ix], sizeof(MemoryInfo));
		if ( strlen(dst.partname) == 0 )
			break;
		uint8_t len;
		for(len = 0; dst.partname[len] != 0 and name[len] != 0 and dst.partname[len] == name[len]; ++len) {}
		if ( dst.partname[len] == 0 and name[len] == 0 ) {
			best = len;
			best_ix = ix;
			break;
		}
		if (len > best ) {
			best = len;
			best_ix = ix;
		}
	}
	if ( best == 0 ) {
		memcpy_P(&dst, &MEMINFO_DB[0], sizeof(MemoryInfo));
		return;
	}
	memcpy_P(&dst, &MEMINFO_DB[best_ix], sizeof(MemoryInfo));
	return;
}

void list_target_types(void) {
	MemoryInfo tmp;
	char buf64[64];
	for (uint8_t ix = 0; ; ++ix) {
		memcpy_P(&tmp, &MEMINFO_DB[ix], sizeof(MemoryInfo));
		if (tmp.partname[0] == '\0' )
			break;
		snprintf(buf64, 64, "%d: %-12s %ldkbits ", ix, tmp.partname, tmp.capacity_inbits / 1024);
		Serial.print(buf64);
		Serial.print(tmp.type);
		Serial.print(" ");
		Serial.print(tmp.page_size);
		Serial.println(tmp.SDP ? " SDP" : "");
	}
	return;
}
