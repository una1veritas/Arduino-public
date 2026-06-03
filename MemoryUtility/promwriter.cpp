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

void list_target_types(MemoryInfo & meminfo) {
	MemoryInfo tmp;
	uint8_t N = 0;
	for(; ; ++N) {
		memcpy_P(&tmp, &MEMINFO_DB[N], sizeof(MemoryInfo));
		if ( tmp.partname[0] == 0 )
			break;
	}
	uint8_t len;
	char buf64[64];
	for (uint8_t ix = 0; ix < N; ++ix) {
		memcpy_P(&tmp, &MEMINFO_DB[ix], sizeof(MemoryInfo));
		snprintf(buf64, 64, "%c%2d %-12s % 4ldk ", (meminfo == tmp ? '*' : ' '), ix, tmp.partname, tmp.capacity_inbits>>13);
		len = Serial.print(buf64);
		switch (tmp.type) {
		case SRAM:
			Serial.print(F("SRAM   "));
			break;
	//	case DRAM:
	//		Serial.print(F("DRAM   "));
	//	case ROM: 		// mask rom
	//		break;
		case EPROM: 	// UV-EPROM
			Serial.print(F("EPROM  "));
			break;
		case EEPROM:		// E-EPROM
			Serial.print(F("EEPROM "));
			break;
		case FLASH:
			Serial.print(F("Flash  "));
			break;
		default:
			Serial.print(F("???    "));
			break;
		}
		len += 7;
//		len += Serial.print(" ");
//		len += Serial.print(tmp.page_size > 0 ? "pw" : "");
//		len += Serial.print(tmp.SDP ? " SDP" : "");
		for(; len < 30; ++len) {
			Serial.print(' ');
		}
		if ( (ix & 1) == 1 )
			Serial.println();
	}
	Serial.println();
	return;
}

void MemoryInfo::printOn(Stream & out) const {
	out.print(partname);
	out.print(F("  "));
	switch (type) {
	case SRAM:
		Serial.print(F("SRAM, "));
		break;
//	case DRAM:
//		Serial.print(F("DRAM, "));
//	case ROM: 		// mask rom
//		break;
	case EPROM: 	// UV-EPROM
		Serial.print(F("UV EPROM, "));
		break;
	case EEPROM:		// E-EPROM
		Serial.print(F("EEPROM, "));
		break;
	case FLASH:
		Serial.print(F("Flash, "));
		break;
	default:
		Serial.print(F("Unknown, "));
		break;
	}
	out.print(capacity_inbits >> 13);
	out.print(F("K bytes, "));
	out.print(F("access time "));
	out.print(access_time);
	out.print(", ");
	if (page_size == 0) {
		out.print(F("no page write"));
	} else {
		out.print(page_size);
		out.print(F(" bytes page write"));
	}
	if (SDP) {
		out.println(F(", has SDP."));
	} else {
		out.println(F("."));
	}
}
