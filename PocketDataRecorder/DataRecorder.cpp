/*
 * DataRecorder.cpp
 *
 *  Created on: 2017/07/11
 *      Author: sin
 */

#include "DataRecorder.h"

void DataRecorder::getE500HeaderInfo() {
	unsigned int i;
	char * ptr;
	dataname[0] = 0;

	datatype = rambuf[0];
	startaddress = 0;
	entryaddress = 0;
	switch(datatype) {
	case DATA_E500_CSAVE_M:
		startaddress = rambuf[0x1E];
		startaddress <<= 8;
		startaddress |= rambuf[0x15];
		startaddress <<= 8;
		startaddress |= rambuf[0x14];
		entryaddress = rambuf[0x1F];
		entryaddress <<= 8;
		entryaddress |= rambuf[0x17];
		entryaddress <<= 8;
		entryaddress |= rambuf[0x16];
		// continue to the next case.
	case DATA_E500_CSAVE:
		datasize = rambuf[0x1D];
		datasize <<= 8;
		datasize |= rambuf[0x13];
		datasize <<= 8;
		datasize |= rambuf[0x12];
		for(i = 0; i < 8; ++i) {
			dataname[i] = rambuf[1+i];
			if ( dataname[i] == 0x20 )
				break;
		}
		dataname[i] = 0;
		break;
	case DATA_E500_CAS:
		datasize = 256;
		for(i = 0; i < 8; ++i) {
			dataname[i] = rambuf[1+i];
			if ( dataname[i] == 0x20 )
				break;
		}
		dataname[i] = 0;
		ptr = dataname + i;
		if ( rambuf[0x0e] != 0x20 ) {
			*ptr++ = '.';
			for(int i = 0; i < 3; ++i) {
				*ptr = rambuf[0x0e + i];
				if ( *ptr == 0x20 )
					break;
				++ptr;
			}
			*ptr = 0;
		}
		break;
	}
}

bool DataRecorder::getByte_E500(byte & b8) {
	unsigned long pulse;
	int ix;
	b8 = 0; // not necessary
	for(ix = 0; ix < 8; ix++) {
		pulse = getPulse();
		if ( pulse == PULSE_IDLE )
			break;
		b8 <<= 1;
		b8 |= (pulse == PULSE_E500_1);
	}
	if ( ix != 8 ) {
		return false;
	}
	if ( getPulse() != PULSE_E500_1 ) {
		return false;
	}
	return true;
}

