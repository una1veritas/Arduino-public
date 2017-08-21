#include <Arduino.h>

#include "SdFat.h"
#include "SPISRAM.h"

class DataRecorder {
	enum PULSE {
	  PULSE_E500_0 = 160, // u sec., 1/2 * 1/3.12kHz
	  PULSE_E500_1 = 407, // 1/2 * 1/1.23kHz
	  PULSE_PC150x_0 = 393, // 1.27kHz
	  PULSE_PC150x_1 = 197, // 2.54kHz
	  PULSE_IDLE = 0,
	  PULSE_ERROR = -1,
	};

	enum PULSE_FORMAT {
		PULSE_FORMAT_E5x0,
		PULSE_FORMAT_PC150x,
		PULSE_FORMAT_UNDETERMINED,
	};

	enum READER_STATE {
	  STANDBY = 0,
// E500 BASIC reader
	  E500_LEAD0_10000,
	  E500_LEAD1_40,
	  E500_LEAD0_40,
	  E500_LEAD1_2,
	  E500_HEADER,
	  E500_INTERMISSION,
	  E500_LEAD0_11000,
	  E500_LEAD1_20,
	  E500_LEAD0_20,
	  E500_LEAD1_11,
	  E500_BODY,
	  E500_BLOCK256, // SAVE"CAS:xxx" mode
// PC150x BASIC READER
  	  PC150x_LEADIN,
	  PC150x_LEADIN_A,
	  PC150x_STARTBIT,
	  PC150x_QUADBIT,
	  PC150x_STOPBITS,

	  READ_COMPLETED,
	  ERROR = 0x8000,
	};

	const int casin, casout;
	SPISRAM & rambuf;
	bool activated;
	PULSE_FORMAT pulseMode;

	byte datatype;
	char dataname[16];
	unsigned long datasize, startaddress, entryaddress;
	unsigned long filesize;

public:
	enum DATA_TYPE {
		DATA_TYPE_EMPTY = 0,
		DATA_E500_CSAVE_M = 0x01,
		DATA_E500_CSAVE = 0x02,
		DATA_E500_CAS = 0x04,
		DATA_E500_DISK_BIN = 0xFF,
		DATA_PC150x_BASIC_IMG = 0x10,
		DATA_ASCII_TEXT = 0x27,
	};

private:
	unsigned int state;
	//PULSE reading;
	//DATA_TYPE readingType;
	//unsigned long pulsecounter;
	//unsigned int bitcounter;
	//unsigned char val;
	bool final_flag;

public:
	DataRecorder(const int cinpin, const int coutpin, SPISRAM & ram) :
		casin(cinpin), casout(coutpin), rambuf(ram) {

		pulseMode = PULSE_FORMAT_UNDETERMINED;

		datatype = DATA_TYPE_EMPTY;
		datasize = 0;
		startaddress = 0;
		entryaddress = 0;
		filesize = 0;

		pinMode(casin, INPUT);
		inactivate();
		state = 0;
		//pulsecounter = 0;
	}

	void activate() {
		pinMode(casout, OUTPUT);
		digitalWrite(casout,LOW);
		activated = true;
	}

	void inactivate() {
		pinMode(casout,INPUT);
		digitalWrite(casout, HIGH); // Weak pull-up
		activated = false;
	}

	bool active() const {
		return activated;
	}

	bool record_completed() const {
		return state == READ_COMPLETED;
	}

	void pulseOut(const PULSE & dur, const unsigned long & repeat = 1) {
		for(unsigned long i = 0; i < repeat; ++i) {
			digitalWrite(casout, HIGH);
			delayMicroseconds(dur);
			digitalWrite(casout, LOW);
			delayMicroseconds(dur);
		}
	}

	PULSE getPulse(void) {
		unsigned long dur = pulseIn(casin, HIGH);
		if ( dur == 0 )
			return PULSE_IDLE;
		switch(pulseMode) {
		case PULSE_FORMAT_E5x0:
		    if ( 150 <= dur && dur < 168 ) { // 160, 3.2kHz
		      return PULSE_E500_0;
		    } else if ( 393 <= dur && dur < 414 ) { // 407, 1.23kHz
		      return PULSE_E500_1;
		    }
		    return PULSE_ERROR;
		    break;
		case PULSE_FORMAT_PC150x:
			if ( 185 <= dur && dur < 207 ) { // 196, 2.54 kHz
				return PULSE_PC150x_1;
			} else if ( 380 <= dur && dur < 405 ) { // 393, 1.25kHz
				return PULSE_PC150x_0;
			}
			return PULSE_ERROR;
			break;
		default:
			if ( 152 <= dur && dur < 168 ) { // 160, 3.2kHz
				return PULSE_E500_0;
			} else if ( 185 <= dur && dur < 207 ) { // 196, 2.54 kHz
				return PULSE_PC150x_1;
			} else if ( 380 <= dur && dur < 396 ) { // 393, 1.25kHz
				return PULSE_PC150x_0;
			} else if ( 400 <= dur && dur < 414 ) { // 407, 1.23kHz
				return PULSE_E500_1;
			}
			return PULSE_ERROR;
			break;
		}
	}

	long countPulses(PULSE pulse, long limit) {
		long count;
		for(count = 0; count < limit; count++) {
			if ( getPulse() != pulse )
				return count;
		}
		return count;
	}

	void reader_reset() {
		state = STANDBY;
		//pulsecounter = 0;
		//bitcounter = 0;
		//readingType = DATA_TYPE_UNKNOWN;
		final_flag = false;
	}

	void record() {
		unsigned long lasted = millis();
		PULSE pulse;
		activate();
		reader_reset();
		rambuf.reset();
		pulseMode = PULSE_FORMAT_UNDETERMINED;
		while (pulseMode == PULSE_FORMAT_UNDETERMINED ) {
			pulse = getPulse();
			switch(pulse) {
			case PULSE_E500_0:
			case PULSE_E500_1:
				pulseMode = PULSE_FORMAT_E5x0;
				state = STANDBY;
				break;
			case PULSE_PC150x_0:
			case PULSE_PC150x_1:
				pulseMode = PULSE_FORMAT_PC150x;
				state = STANDBY;
				break;
			}
			if ( millis() - lasted > 30000 ) {
				break;
			}
		}
		if ( pulseMode == PULSE_FORMAT_E5x0 ) {
			state = record_E5x0();
			if ( state & ERROR ) {
				Serial.println("ERROR.");
			}
		} else if ( pulseMode == PULSE_FORMAT_PC150x ) {
			state = record_PC150x();
			if ( state & ERROR ) {
				Serial.println("ERROR.");
			}
		}

		inactivate();
	}

	bool getByte_E500(byte &);

	void getE500HeaderInfo();

	READER_STATE record_E5x0(void) {
		long count;
		int ti;
		byte ub;
		bool termFlag = false;

		Serial.println("reading header...");
		if ( countPulses(PULSE_E500_0, 20000) < 9996 ) {
			return ERROR | E500_LEAD0_10000;
		}
		// already lost one PULSE_E500_1 in breaking out counting E500_0
		if ( countPulses(PULSE_E500_1, 39) < 39 ) {
			return ERROR | E500_LEAD1_40;
		}
		if ( countPulses(PULSE_E500_0, 40) != 40 ) {
			return ERROR | E500_LEAD0_40;
		}
		if ( countPulses(PULSE_E500_1, 2) != 2 ) {
			return ERROR | E500_LEAD1_2;
		}
		// file info header
		for(count = 0; count < 0x32; count++) {
			if ( !getByte_E500(ub) )
				break;
			rambuf.write(ub);
		} //while ( rambuf.available() < 0x32 ); // while (true) should be sufficient
		if ( count < 0x32 ) {
			return ERROR | E500_HEADER;
		}

		getE500HeaderInfo();
		Serial.print("type ");
		Serial.print(datatype, HEX);
		Serial.print(", name '");
		Serial.print(dataname);
		Serial.print("', size ");
		Serial.print(datasize);
		Serial.print(", start address ");
		Serial.print(startaddress);
		Serial.print(", entry address ");
		Serial.print(entryaddress);
		Serial.println();

		termFlag = false;
		do {
			// wait for E500_0
			if ( countPulses(PULSE_IDLE, 20000) >= 5000 ) {
				//Serial.println("too long intermission");
				return ERROR | E500_INTERMISSION;
			}
			//Serial.println("body leader");
			// E500_0, 11000 (PC-E500) ~ 25848 (PC-G850VS)
			if ( countPulses(PULSE_E500_0, 25856) < 10998 ) {
				return ERROR | E500_LEAD0_11000;
			}
			// E500_1, 20
			if ( countPulses(PULSE_E500_1, 19) != 19 ) {
				return ERROR | E500_LEAD1_20;
			}
			// E500_0, 20
			if ( countPulses(PULSE_E500_0, 20) != 20 ) {
				return ERROR | E500_LEAD0_20;
			}
			if ( datatype == DATA_E500_CSAVE ) {
				// E500_1, 11
				if ( countPulses(PULSE_E500_1, 11) != 11 ) {
					return ERROR | E500_LEAD1_11;
				}
			} else if ( datatype == DATA_E500_CSAVE_M || datatype == DATA_E500_CAS ) {
				// E500_1, 2
				if ( countPulses(PULSE_E500_1, 2) != 2 ) {
					return ERROR | E500_LEAD1_11;
				}
			}
			// data block
			Serial.println("reading data block...");
			for( count = 0; count < datasize; count++) {
				if ( !getByte_E500(ub) )
					break;
				if (datatype == DATA_E500_CSAVE_M || datatype == DATA_E500_CSAVE )
					rambuf.write(ub);
				else if ( datatype == DATA_E500_CAS && !termFlag )
					rambuf.write(ub);
				if ( datatype == DATA_E500_CAS && ub == 0x1A ) {
					termFlag = true;
				}
			}// while ( count <= datasize ); // while (true) should be sufficient
			//Serial.println(count);
			if ( count < datasize ) {
				return ERROR | E500_BODY;
			}
			if ( datatype == DATA_E500_CSAVE || datatype == DATA_E500_CSAVE_M ) {
				termFlag = true;
			} else if ( datatype == DATA_E500_CAS ) {
				if ( !getByte_E500(ub) ) {
					//Serial.println("257th byte error");
					return ERROR | E500_BLOCK256;
				}
				count = ub;
				count <<= 8;
				if ( !getByte_E500(ub) ) {
					//Serial.println("258th byte error");
					return ERROR | E500_BLOCK256;
				}
				count |= ub; // check sum ( popl. count)
			}
		} while ( !termFlag );
		Serial.println("finished.");
		return READ_COMPLETED;
	}
/*
	void record_E5x0(PULSE pulse) {
		long cnt;
		switch (state) {
		case STANDBY:
			// ignore PULSE_IDLE
			if (pulse == PULSE_E500_0) {
				state = E500_LEAD0_10000;
				pulsecounter = 1;
				//Serial.println(F(" -> LEAD0_10000"));
				return;
			}
			break;
		case E500_LEAD0_10000:
			if (pulse == PULSE_E500_0) {
				pulsecounter++;
				return;
			} else if (pulse == PULSE_E500_1 && pulsecounter >= 9600 ) {
				//Serial.println(pulsecounter);
				state = E500_LEAD1_40;
				pulsecounter = 1;
				return;
			}
			break;
		case E500_LEAD1_40:
			if (pulse == PULSE_E500_1) {
				pulsecounter++;
				if (pulsecounter >= 40) {
					state = E500_LEAD0_40;
					pulsecounter = 0;
					//Serial.println(F(" -> LEAD0_40"));
				}
				return;
			}
			break;
		case E500_LEAD0_40:
			if (pulse == PULSE_E500_0) {
				pulsecounter++;
				if (pulsecounter >= 40) {
					state = E500_LEAD1_2;
					pulsecounter = 0;
					//Serial.println(" -> LEAD1_2");
				}
				return;
			}
			break;
		case E500_LEAD1_2:
			if (pulse == PULSE_E500_1) {
				pulsecounter++;
				if (pulsecounter >= 2) {
					state = E500_HEADER;
					pulsecounter = 0;
					//Serial.println(F(" -> HEADER"));
				}
				return;
			}
			break;
		case E500_HEADER:
			if ( pulsecounter < 8 && (pulse == PULSE_E500_0 || pulse == PULSE_E500_1) ) {
				val <<= 1;
				val |= (pulse == PULSE_E500_1);
				++pulsecounter;
				return;
			} else if (pulsecounter == 8 && pulse == PULSE_E500_1 ) {
				rambuf.write(val);
				//bytecounter++;
				pulsecounter = 0;
				return;
			} else if (pulse == PULSE_IDLE) {
				pulsecounter = 0;
				state = E500_INTERMISSION;

				byte t = rambuf[0];
				Serial.print("type = "); Serial.println(t, HEX);
				Serial.print("'");
				for(int i = 0; i < 16; i++) {
					Serial.print((char)rambuf[1+i]);
				}
				Serial.println("'");

				//Serial.print(F("INTERMISSION "));
				return;
			}
			break;
		case E500_INTERMISSION:
			if (pulse == PULSE_IDLE) {
				pulsecounter++;
				return;
			} else if (pulse == PULSE_E500_0) {
				state = E500_LEAD0_11000;
				pulsecounter = 1;
				//Serial.println(F(" -> LEAD0_11000"));
				return;
			}
			break;
		case E500_LEAD0_11000:  // PC-G850xx outpus 25848 zero's here.
			if (pulse == PULSE_E500_0) {
				pulsecounter++;
				return;
			} else if ( pulse == PULSE_E500_1 && pulsecounter >= 10000 ) {
				state = E500_LEAD1_20;
				pulsecounter = 1;
				Serial.println(F(" -> LEAD1_20"));
				return;
			}
			break;
		case E500_LEAD1_20:
			if (pulse == PULSE_E500_1) {
				pulsecounter++;
				if (pulsecounter >= 20) {
					state = E500_LEAD0_20;
					pulsecounter = 0;
					Serial.println(F(" -> LEAD0_20"));
				}
				return;
			}
			break;
		case E500_LEAD0_20:
			if (pulse == PULSE_E500_0) {
				pulsecounter++;
				if (pulsecounter >= 20) {
					state = E500_LEAD1_11;
					pulsecounter = 0;
					Serial.println(" -> LEAD1_11");
				}
				return;
			}
			break;
		case E500_LEAD1_11:
			if (pulse == PULSE_E500_1) {
				pulsecounter++;
				DATA_TYPE t = rambuf[0];
				if ( (t == DATA_E500_CSAVE_M || t == DATA_E500_CAS) && pulsecounter == 2 ) {
					state = E500_BLOCK256;
					pulsecounter = 0;
					Serial.println(" -> BLOCK256");
				} else if ( t == DATA_E500_CSAVE && pulsecounter >= 11) {
					state = E500_BODY;
					pulsecounter = 0;
					Serial.println(" -> BODY");
				}
				return;
			}
			break;
		case E500_BODY:
		case E500_BLOCK256:
			if (pulse == PULSE_E500_0 || pulse == PULSE_E500_1) {
				if (pulsecounter < 8) {
					val <<= 1;
					val |= (pulse == PULSE_E500_1);
					pulsecounter++;
					return;
				} else {
					if (pulse == PULSE_E500_1) {
						rambuf.write(val);
						//bytecounter++;
						if ( val == 0x1a ) {
							final_flag = true;
						}
						pulsecounter = 0;
						return;
					}
				}
			} else if (pulse == PULSE_IDLE) {
				Serial.println();
				pulsecounter = 0;
				DATA_TYPE t = rambuf[0];
				if ( t == DATA_E500_CSAVE || t == DATA_E500_CSAVE_M ) {
					state = READ_COMPLETED;
					bytesize = rambuf.available();
					return;
				} else if ( t == DATA_E500_CAS ) {
					if ( final_flag ) {
						state = READ_COMPLETED;
						bytesize = rambuf.available();
					} else {
						state = E500_INTERMISSION;
					}
					return;
				}
			}
			break;
		}
		state = ERROR;
		Serial.println("ERROR.");
	}
*/
	unsigned char imageType() const {
		return datatype;
	}

	unsigned long imageSize() const {
		return datasize;
	}

	const char * imageName() {
		return dataname;
	}

	void fileOut(File & file, const unsigned long offset = 0) {
		//Serial.println(F("fileout img."));
		unsigned long bufsize = rambuf.available();
		for( unsigned long ix = offset; ix < bufsize; ++ix ) {
			file.write(rambuf[ix]);
		}
		return;
	}

	static unsigned int popcount(unsigned char val) {
		unsigned int sum = 0;
		while ( val != 0 ) {
			sum += (val & 1);
			val >>= 1;
		}
		return sum;
	}

	unsigned long fileIn(File & file) {
		unsigned int chksum;

		if ( file.available() > 16 ) {
			datatype = (byte) file.peek();
		} else {
			// error
			Serial.println(F("not available."));
			filesize = 0;
			datasize = 0;
			return filesize;
		}
		reader_reset();
		rambuf.reset();

		if ( datatype == DATA_E500_CSAVE
				|| datatype == DATA_E500_CSAVE_M
				|| datatype == DATA_PC150x_BASIC_IMG ) {
			// raw data type
			Serial.println("CSAVE/CSAVE M/PC-150x raw file");
		} else if ( datatype == DATA_E500_DISK_BIN ) {
			Serial.println("E500 binary disk file");
			// FF, then 00 06 01 10
			for(int i = 0; i < 4 ; ++i )
				file.read();
			datasize = file.read();
			datasize |= ((unsigned long)file.read()) << 8;
			datasize |= ((unsigned long)file.read()) << 16;
			startaddress = file.read();
			startaddress |= ((unsigned long)file.read()) << 8;
			startaddress |= ((unsigned long)file.read()) << 16;
			// 0x0B - 0x0F: FF FF FF 00 0F
			for(int i = 0; i < 5 ; ++i )
				file.read();
			/*
			 * 0x00: 0x01 type
			 * 0x01 --- 0x10: file name (max 8 chars) 16 bytes filled with space 0x20
			 * 0x11: 0x0D
			 * 0x12, 0x13: length low, mid 2 bytes
			 * 0x14, 0x15: load addr. low, mid 2 bytes
			 * 0x16, 0x17: call addr. low, mid 2 bytes
			 * 0x1D: length high 1 byte
			 * 0x1E: load addr. high 1 byte
			 * 0x1F: call addr. high 1 byte
			 * 0x31: check sum (popl. count)
			 */
			datatype = DATA_E500_CSAVE_M; // after conversion
			rambuf.write(datatype);
			for(int i = 0; i < 16; i++)
				rambuf.write(0x20);
			rambuf.write(0x0D);
			rambuf.write((byte)(datasize & 0xff));rambuf.write((byte)(datasize>>8 & 0xff));
			rambuf.write((byte)(startaddress & 0xff));rambuf.write((byte)(startaddress>>8 & 0xff));
			rambuf.write((byte)(0 & 0xff));rambuf.write((byte)(0>>8 & 0xff));
			for(int i = 0; i < 5; i++)
				rambuf.write(0);
			rambuf.write(datasize>>16 & 0xff);
			rambuf.write(startaddress>>16 & 0xff);
			rambuf.write(0>>16 & 0xff);
			for(int i = 0; i < 18; i++)
				rambuf.write(0);
			chksum = 0;
			for(datasize = 0; datasize < 0x30; datasize++ ) {
				//Serial.print("addr ");Serial.print(datasize, HEX); Serial.print(" : "); Serial.print(rambuf[datasize], HEX);
				//Serial.print(" popl "); Serial.print(popcount(rambuf[datasize]), HEX);
				chksum += popcount(rambuf[datasize]);
				//Serial.print(" sum ");Serial.println(chksum, HEX);
			}
			rambuf.write(chksum>>8 & 0xff);
			rambuf.write(chksum & 0xff);
		} else if ( isprint(datatype) ) {
			Serial.println(F("ascii text still not supported."));
			// ascii text file
			// still not supported
			// datatype = DATA_ASCII_TEXT;
		}

		while ( file.available() ) {
			rambuf.write((unsigned char) file.read());
		}
		filesize = rambuf.available();
		if ( datatype == DATA_E500_CSAVE || datatype == DATA_E500_CSAVE_M ) {
			datatype = filesize - 50;
		}
		return filesize;
	}

	/*
	void record_PC150x(PULSE pulse) {
		switch (state) {
		case STANDBY:
			if (pulse == PULSE_PC150x_1) {
				pulsecounter = 1;
				state = PC150x_LEADIN;
			}
			break;
		case PC150x_LEADIN:
			if (pulse == PULSE_PC150x_1) {
				pulsecounter++;
			} else if (pulse == PULSE_PC150x_0) {
				if ( pulsecounter >= 2540 ) {
					//Serial.print("LEADIN (");
					//Serial.print(pulsecounter);
					//Serial.println(") -> STARTBIT");
					state = PC150x_STARTBIT;
					rambuf.reset();
					pulsecounter = 1;
					bitcounter = 4;
				} else {
					state = ERROR;
				}
			} else {
				state = ERROR;
			}
			break;
		case PC150x_STARTBIT:
			if (pulse == PULSE_PC150x_0) {
				pulsecounter++;
				if ( pulsecounter == 4 ) {
					state = PC150x_QUADBIT;
					pulsecounter = 0;
				}
			} else {
				state = ERROR;
			}
			break;
		case PC150x_QUADBIT:
			if (pulsecounter == 0) {
				reading = pulse;
				pulsecounter = 1;
			} else if ( reading == pulse ) {
				pulsecounter++;
				if ( bit0complete() || bit1complete()) {
					if ( reading == PULSE_PC150x_0 ) {
						bitIn(0);
						// Serial.print("0");
					} else if ( reading == PULSE_PC150x_1 ) {
						bitIn(1);
						// Serial.print("1");
					}
					if ( (bitcounter & 0x03) == 0 ) {
						// Serial.print(":");
						// Serial.print(pc1500.bcount);
						// Serial.print(".");
						if ( bitcounter == 8 ) {

							Serial.print( val >> 4 & 0x0f, HEX );
							Serial.print( val & 0x0f, HEX );
							Serial.print(" ");

							rambuf.write(val);
							//bytecounter++;
							val = 0;
							bitcounter = 0;

						}
						state = PC150x_STOPBITS;
					}
				}
			} else {
				state = ERROR;
				Serial.print("QUADBIT -> ERROR");
			}
			break;
		case PC150x_STOPBITS:
			if (pulse == PULSE_PC150x_1) {
				pulsecounter++;
			} else if (pulse == PULSE_PC150x_0) {

				//if ( pulsecounter != 48 ) {
				//	Serial.print("STOP BITS (");
				//	Serial.print(pulsecounter);
				//	Serial.println(") ");
				//}

				state = PC150x_STARTBIT;
				pulsecounter = 1;
				//Serial.print("T ");
			} else if (pulse == PULSE_IDLE) {
				//Serial.print("STOP BITS (");
				//Serial.print(pulsecounter);
				//Serial.println(") -> END ");
				state = READ_COMPLETED;
				Serial.println();
				Serial.println("READ_COMPLETED");
			} else {
				Serial.print("X");
				state = ERROR;
			}
			break;

		case ERROR:
			Serial.println();
			Serial.println("ERROR");
			break;
		}
	}
*/

	READER_STATE record_PC150x(void) {
		PULSE pulse;
		byte val = 0;
		unsigned int bitcounter = 4; // ignore the first quad-bit

		if ( countPulses(PULSE_PC150x_1, 2540) < 2500 ) {
			return ERROR | PC150x_LEADIN;
		}

		do {
			// already one 0 bit has been passed
			if ( countPulses(PULSE_PC150x_0, 3) != 3 ) {
				return ERROR | PC150x_STARTBIT;
			}

			for(unsigned int i = 0; i < 4; i++) {
				pulse = getPulse();
				if ( pulse == PULSE_PC150x_0
						&& countPulses(PULSE_PC150x_0, 3) == 3 ) {
					val >>= 1;
					bitcounter++;
				} else if ( pulse == PULSE_PC150x_1
						&& countPulses(PULSE_PC150x_1, 7) == 7 ) {
					val >>= 1;
					val |= 0x80;
					bitcounter++;
				} else {
					// error
					return ERROR | PC150x_QUADBIT;
				}
			}
			if ( bitcounter == 8 ) {
				rambuf.write(val);
				val = 0;
				bitcounter = 0;
			}

			//PC150x_STOPBITS:
			do {
				pulse = getPulse();
			} while ( pulse == PULSE_PC150x_1 );
			if ( pulse == PULSE_IDLE )
				break;
		} while (true);

		Serial.println("finished.");
		return READ_COMPLETED;
	}
/*
	bool bit0complete() {
		if ( reading == PULSE_PC150x_0 && pulsecounter == 4 ) {
			pulsecounter = 0;
			return true;
		}
		return false; // error
	}

	bool bit1complete() {
		if ( reading == PULSE_PC150x_1 && pulsecounter == 8 ) {
			pulsecounter = 0;
			return true;
		}
		return false; // error
	}

	void bitIn(unsigned int b) {
		val >>= 1;
		if ( b != 0 )
			val |= 0x80;
		++bitcounter;
	}
*/

	void play() {
		activate();
		unsigned char type = rambuf[0];
		if ( type == DATA_PC150x_BASIC_IMG ) { // CLOAD
			play_PC150x(imageSize());
		} else if ( type == DATA_E500_CSAVE || type == DATA_E500_CSAVE_M ) {
			Serial.println("CLOAD.");
			play_E5x0(type); // CLOAD
		} else if ( type == DATA_E500_CAS ) {
			play_E5x0(DATA_E500_CAS); // LOAD "CAS:
		}
		inactivate();
	}

	void byteOut_E5x0(byte b) {
		for(int i = 0; i < 8; ++i) {
			if ( (b & 0x80) == 0 ) {
				pulseOut(PULSE_E500_0);
			} else {
				pulseOut(PULSE_E500_1);
			}
			b <<= 1;
		}
		pulseOut(PULSE_E500_1);
	}

	void play_E5x0(const unsigned char type) {
		unsigned int addr, blk;

		// E500_LEAD0_10000:
		Serial.print("filesize = ");
		Serial.println(filesize, DEC);
		delay(1000);
		pulseOut(PULSE_E500_0, 10000);
		// E500_LEAD1_40:
		pulseOut(PULSE_E500_1, 40);
		// E500_LEAD0_40:
		pulseOut(PULSE_E500_0, 40);
		// E500_LEAD1_2:
		pulseOut(PULSE_E500_1, 2);
		// E500_TEXT1_9:
		addr = 0;
		while ( addr < 50 ) {
			byte t = rambuf.read(addr++);
			byteOut_E5x0(t);
//			Serial.print(t>>4&0x0f, HEX);
//			Serial.print(t&0x0f, HEX);
//			Serial.print(' ');
		}
		Serial.println();

		do {
			// E500_INTERMISSION:
			delay(2400);
			// E500_LEAD0_11000:  // PC-G850xx outpus 25848 zero's here.
			pulseOut(PULSE_E500_0, 11000);
			// E500_LEAD1_20:
			pulseOut(PULSE_E500_1, 20);
			// E500_LEAD0_20:
			pulseOut(PULSE_E500_0, 20);
			// E500_LEAD1_11:
			if ( type == DATA_E500_CSAVE ) {
				pulseOut(PULSE_E500_1, 11);
			} else if ( type == DATA_E500_CAS || type == DATA_E500_CSAVE_M ) {
				pulseOut(PULSE_E500_1, 2);
			}
			// E500_TEXT2_9:
			if ( type == DATA_E500_CSAVE || type == DATA_E500_CSAVE_M) {
				while ( addr < filesize ) {
					//byte t = rambuf.read(i);
					byteOut_E5x0(rambuf.read(addr++));
					//Serial.print(t, HEX);
					//Serial.print(' ');
				}
				break;
			} else if ( type == DATA_E500_CAS ) {
				for(int i = 0; i < 258; ++i) {
					byte t = rambuf.read(addr+i);
					byteOut_E5x0(t);
					//Serial.print(t, HEX);
					//Serial.print(' ');
				}
				addr += 258;
				Serial.print(addr, DEC);
				Serial.println();
			}
		} while (addr < filesize);
		delay(1000);
	}

	void play_PC150x(long bytes) {
		unsigned int length = bytes;
		int i;

		//length = 0x2a + ((ssram.read(0x24)<<8) + ssram.read(0x25)) + 4;

		for (i = 0; i < 10196 /* 20392*/; ++i) {
			pulseOut(PULSE_PC150x_1);
		}
		quadOut(0x0A);
		for (i = 0; i < 42; ++i) {
			byteOut_PC150x(rambuf.read(i));
		}
		// long stop bits 632 ~ 640
		for (i = 0; i < 640 - 48; ++i) {
			pulseOut(PULSE_PC150x_1);
		}
		for (i = 42; i < length - 1; ++i) {
			byteOut_PC150x(rambuf.read(i));
		}
		// long stop bits 600 ~ 640
		for (i = 0; i < 640 - 48; ++i) {
			pulseOut(PULSE_PC150x_1);
		}
		byteOut_PC150x(rambuf.read(length-1));
		// long stop bits 618 ~ 623
		for (i = 0; i < 624 - 48; ++i) {
			pulseOut(PULSE_PC150x_1);
		}

	}

	void quadOut(unsigned char qval) {
		for(int i = 0; i < 4; ++i)
			pulseOut(PULSE_PC150x_0);
		for(int i = 0; i < 4; ++i) {
			if ( (qval & 0x01) == 1 ) {
				for(int t = 0; t < 8; ++t)
					pulseOut(PULSE_PC150x_1);
			} else {
				for(int t = 0; t < 4; ++t)
					pulseOut(PULSE_PC150x_0);
			}
			qval >>= 1;
		}
		for(int i = 0; i < 6; ++i ) {
			for(int t = 0; t < 8; ++t)
				pulseOut(PULSE_PC150x_1);
		}
	}

	void byteOut_PC150x(byte val) {
		quadOut(val & 0x0f);
		quadOut(val>>4 & 0x0f);
	}

};

