#include <string.h>

#include <SPI.h>
#include <SPISRAM.h>
#include <SD.h>

#include "pins.h"


enum PULSE {
  PULSE_E500_0 = 156,
  PULSE_E500_1 = 205,
  PULSE_IDLE = 2,
  PULSE_PC15_1 = 196,
  PULSE_PC15_0 = 393,
  PULSE_ERR = -1,
};

struct PCDataRecorder {
	enum STATUS {
		STOP,
		STANDBY,
		PLAY,
		RECORD,
		CUEING,
	} state;

	enum TARGETMACHINE {
		SHARP_PC150x,
		SHARP_E5x0,
		CASIO_FX870P,
		CASIO_FX880P,
	} model;

	SDClass & SD;
	SPISRAM & sram;

	PCDataRecorder(SDClass & sd, SPISRAM & ram) :
		state(STOP), model(SHARP_PC150x), SD(sd), sram(ram) {}
};

SPISRAM ssram(SRAM_CS_PC6, SPISRAM::BUS_MBits); // CS pin
File sdfile;
PCDataRecorder datarec(SD,ssram);

const unsigned long DURATION_IDLE = 5000; // micro sec.

int getPulse() {
  int duration = pulseIn(XOUT_PD0, HIGH, DURATION_IDLE);
  if ( 153 <= duration && duration < 168 ) { // 160, 3.2kHz
    return PULSE_E500_0;
  } else if ( 400 <= duration && duration < 420 ) { // 410, 2.44kHz
    return PULSE_E500_1;
  } else if (duration == 0 ) {
    return PULSE_IDLE;
  } else if ( 185 <= duration && duration < 198 ) { // 196
    return PULSE_PC15_1;
  } else if ( 380 <= duration && duration < 396 ) { // 393
    return PULSE_PC15_0;
  } else {
    return -duration; //PULSE_ERR;
  }
}

void pulseOut(unsigned int dur) {
	digitalWrite(XIN_PD1, HIGH);
	delayMicroseconds(dur);
	digitalWrite(XIN_PD1, LOW);
	delayMicroseconds(dur);
}

void quadOut(unsigned char qval) {
	for(int i = 0; i < 4; ++i)
		pulseOut(PULSE_PC15_0);
	for(int i = 0; i < 4; ++i) {
		if ( (qval & 0x01) == 1 ) {
			for(int t = 0; t < 8; ++t)
				pulseOut(PULSE_PC15_1);
		} else {
			for(int t = 0; t < 4; ++t)
				pulseOut(PULSE_PC15_0);
		}
		qval >>= 1;
	}
	for(int i = 0; i < 6; ++i ) {
		for(int t = 0; t < 8; ++t)
			pulseOut(PULSE_PC15_1);
	}
}

void byteOut(unsigned char val) {
	quadOut(val & 0x0f);
	quadOut(val>>4 & 0x0f);
}

struct E500Recorder {
	enum STATE {
	  STANDBY = 0,
	  LEAD0_10000,
	  LEAD1_40,
	  LEAD0_40,
	  LEAD1_2,
	  TEXT1_9,
	  INTERMISSION,
	  LEAD0_11000,
	  LEAD1_20,
	  LEAD0_20,
	  LEAD1_11,
	  TEXT2_9,
	  END,
	  ERR,
	};
  const unsigned long IDLE_ABANDON  = 2000;  // 

  int state;
  unsigned long counter;
  unsigned int text_counter;
  unsigned int text1_size;
  unsigned int text2_size;

  E500Recorder() : state(STANDBY), counter(0), text_counter(0),
  text1_size(0), text2_size(0) {}

	void xin_pulses(PULSE pulse) {
		unsigned int data;

		switch (state) {
		case STANDBY:
			if (pulse == PULSE_IDLE) {
				counter++;
				if (counter > IDLE_ABANDON) {
					Serial.println("IDLE.");
					counter = 0;
				}
			} else if (pulse == PULSE_E500_0) {
				state = LEAD0_10000;
				counter = 1;
				Serial.println(" -> LEAD0_10000");
			} else {
				state = ERR;
			}
			break;
		case LEAD0_10000:
			if (pulse == PULSE_E500_0) {
				counter++;
				if (counter >= 10000) {
					state = LEAD1_40;
					counter = 0;
					Serial.println(" -> LEAD1_40");
				}
			} else {
				state = ERR;
			}
			break;
		case LEAD1_40:
			if (pulse == PULSE_E500_1) {
				counter++;
				if (counter >= 40) {
					state = LEAD0_40;
					counter = 0;
					Serial.println(" -> LEAD0_40");
				}
			} else {
				state = ERR;
			}
			break;
		case LEAD0_40:
			if (pulse == PULSE_E500_0) {
				counter++;
				if (counter >= 40) {
					state = LEAD1_2;
					counter = 0;
					Serial.println(" -> LEAD1_2");
				}
			} else {
				state = ERR;
			}
			break;
		case LEAD1_2:
			if (pulse == PULSE_E500_1) {
				counter++;
				if (counter >= 2) {
					state = TEXT1_9;
					text_counter = 0;
					counter = 0;
					Serial.println(" -> TEXT1_9");
				}
			} else {
				state = ERR;
			}
			break;
		case TEXT1_9:
			if (counter == 0)
				data = 0;
			if (pulse == PULSE_E500_0 || pulse == PULSE_E500_1) {
				counter++;
				if (counter == 9) {
					if (pulse == PULSE_E500_0) {
						Serial.println();
						state = ERR;
					} else {
						ssram.write(text_counter, data);
						text_counter++;
					}
					counter = 0;
				} else {
					data <<= 1;
					data |= (pulse == PULSE_E500_1);
				}
			}
			if (pulse == PULSE_IDLE) {
				counter = 0;
				text1_size = text_counter;
				state = INTERMISSION;
				Serial.println();
			}
			break;
		case INTERMISSION:
			if (pulse == PULSE_IDLE) {
				counter++;
			} else if (pulse == PULSE_E500_0) {
				state = LEAD0_11000;
				counter = 1;
				Serial.println(" -> LEAD0_11000");
			}
			break;
		case LEAD0_11000:
			if (pulse == PULSE_E500_0) {
				counter++;
				if (counter >= 11000) {
					state = LEAD1_20;
					counter = 0;
					Serial.println(" -> LEAD1_20");
				}
			} else {
				Serial.println("LEAD0_11000 failed.");
				state = ERR;
				counter = 0;
			}
			break;
		case LEAD1_20:
			if (pulse == PULSE_E500_1) {
				counter++;
				if (counter >= 20) {
					state = LEAD0_20;
					counter = 0;
					Serial.println(" -> LEAD0_20");
				}
			} else {
				state = ERR;
				counter = 0;
			}
			break;
		case LEAD0_20:
			if (pulse == PULSE_E500_0) {
				counter++;
				if (counter >= 20) {
					state = LEAD1_11;
					counter = 0;
					Serial.println(" -> LEAD1_11");
				}
			} else {
				state = ERR;
				counter = 0;
			}
			break;
		case LEAD1_11:
			if (pulse == PULSE_E500_1) {
				counter++;
				if (counter >= 11) {
					state = TEXT2_9;
					text_counter = text1_size;
					counter = 0;
					Serial.println(" -> TEXT2_9");
				}
			} else {
				state = ERR;
				counter = 0;
			}
			break;
		case TEXT2_9:
			if (counter == 0)
				data = 0;
			if (pulse == PULSE_E500_0 || pulse == PULSE_E500_1) {
				counter++;
				if (counter == 9) {
					if (pulse == PULSE_E500_0) {
						Serial.println();
						state = ERR;
					} else {
						ssram.write(text_counter, data);
						text_counter++;
					}
					counter = 0;
				} else {
					data <<= 1;
					data |= (pulse == PULSE_E500_1);
				}
			}
			if (pulse == PULSE_IDLE) {
				Serial.println();
				text2_size = text_counter - text1_size;
				counter = 0;
				state = END;
			}
			break;

		case ERR:
			counter = 0;
			state = STANDBY;
			Serial.println("ERR");
			break;
		case END:
			Serial.println("TEXT1: ");
			for (int i = 0; i < text1_size; ++i) {
				data = ssram.read(i);
				Serial.print(data >> 4 & 0x0f, HEX);
				Serial.print(data & 0x0f, HEX);
				Serial.print(' ');
			}
			Serial.println();
			Serial.println("TEXT2: ");
			for (int i = 0; i < text2_size; ++i) {
				data = ssram.read(text1_size + i);
				Serial.print(data >> 4 & 0x0f, HEX);
				Serial.print(data & 0x0f, HEX);
				Serial.print(' ');
			}
			Serial.println();

			char fname[24];
			char c;
			int i;
			for (i = 0; i < 8; ++i) {
				fname[i] = ssram.read(i + 1);
				if (fname[i] == 0x20 || fname[i] == 0x0D)
					break;
			}
			fname[i] = 0;
			if (strlen(fname) == 0) {
				strcpy(fname, "TEMPE50");
			}
			strcat(fname, ".REC");
			sdfile = SD.open(fname, FILE_WRITE);

			// if the file is available, write to it:
			if (sdfile) {
				for (int i = 0; i < text1_size; ++i) {
					data = ssram.read(i);
					sdfile.print(data >> 4 & 0x0f, HEX);
					sdfile.print(data & 0x0f, HEX);
					sdfile.print(' ');
				}
				sdfile.println();
				for (int i = 0; i < text2_size; ++i) {
					data = ssram.read(text1_size + i);
					sdfile.print(data >> 4 & 0x0f, HEX);
					sdfile.print(data & 0x0f, HEX);
					sdfile.print(' ');
				}
				sdfile.println();
				sdfile.close();
				Serial.println("saved.");
			}
			// if the file isn't open, pop up an error:
			else {
				Serial.print("error opening '");
				Serial.print(fname);
				Serial.println("'");
			}

			Serial.println("go STANDBY");
			state = STANDBY;
			break;
		default:
			counter = 0;
			state = STANDBY;
			Serial.println("go STANDBY");
			break;
		}
	}
  
};

struct PC1500Recorder {
	enum STATES {
  	  STANDBY,
  	  LEADIN,
  	  LEADIN_A,
  	  STARTBIT,
	  QUADBIT,
  	  STOPBITS,
  	  DATA_END,
	  ERROR,
	} state;

	enum BPOS {
		PROGNAME = 10,
	};

	unsigned int pcount;
	unsigned int bcount;
	unsigned int reading;
	unsigned int val;

	unsigned int txcounter;

	PC1500Recorder() {
		reset();
	}

	void reset() {
		state = STANDBY;
		pcount = 0;
		bcount = 4; // set the nibble bit
		val = 0;
		txcounter = 0;
	}

	bool bit0complete() {
		if ( reading == PULSE_PC15_0 && pcount == 4 ) {
			pcount = 0;
			return true;
		}
		return false; // error
	}

	bool bit1complete() {
		if ( reading == PULSE_PC15_1 && pcount == 8 ) {
			pcount = 0;
			return true;
		}
		return false; // error
	}

	void bitIn(unsigned int b) {
		val >>= 1;
		if ( b != 0 )
			val |= 0x80;
		++bcount;
	}

	bool read_finished() {
		return state == DATA_END;
	}

	void xin_pulses(PULSE pulse) {
		switch (state) {
		case STANDBY:
			if (pulse == PULSE_PC15_1) {
				pcount = 1;
				state = LEADIN;
			}
			break;
		case LEADIN:
			if (pulse == PULSE_PC15_1) {
				pcount++;
			} else if (pulse == PULSE_PC15_0) {
				if ( pcount >= 2540 ) {
					Serial.print("LEADIN (");
					Serial.print(pcount);
					Serial.println(") -> STARTBIT");
					state = STARTBIT;
					pcount = 1;
					bcount = 4;
				} else {
					state = ERROR;
				}
			} else {
				state = ERROR;
			}
			break;
		case STARTBIT:
			if (pulse == PULSE_PC15_0) {
				pcount++;
				if ( pcount == 4 ) {
					state = QUADBIT;
					pcount = 0;
				}
			} else {
				state = ERROR;
			}
			break;
		case QUADBIT:
			if (pcount == 0) {
				reading = pulse;
				pcount = 1;
			} else if ( reading == pulse ) {
				pcount++;
				if ( bit0complete() || bit1complete()) {
					if ( reading == PULSE_PC15_0 ) {
						bitIn(0);
						// Serial.print("0");
					} else if ( reading == PULSE_PC15_1 ) {
						bitIn(1);
						// Serial.print("1");
					}
					if ( (bcount & 0x03) == 0 ) {
						// Serial.print(":");
						// Serial.print(pc1500.bcount);
						// Serial.print(".");
						if ( bcount == 8 ) {

							Serial.print( val >> 4 & 0x0f, HEX );
							Serial.print( val & 0x0f, HEX );
							Serial.print(" ");

							ssram.write(txcounter, val);
							txcounter++;
							val = 0;
							bcount = 0;

						}
						state = STOPBITS;
					}
				}
			} else {
				state = ERROR;
				Serial.print("QUADBIT -> ERROR");
			}
			break;
		case STOPBITS:
			if (pulse == PULSE_PC15_1) {
				pcount++;
			} else if (pulse == PULSE_PC15_0) {
				if ( pcount != 48 ) {
					Serial.print("STOP BITS (");
					Serial.print(pcount);
					Serial.println(") ");
				}
				state = STARTBIT;
				pcount = 1;
				//Serial.print("T ");
			} else if (pulse == PULSE_IDLE) {
				Serial.print("STOP BITS (");
				Serial.print(pcount);
				Serial.println(") -> END ");
				state = DATA_END;
				Serial.println();
				Serial.println("DATA_END");
			} else {
				Serial.print("X");
				state = ERROR;
			}
			break;

		case PC1500Recorder::ERROR:
			Serial.println();
			Serial.println("ERROR");
			reset();
			break;
		}
	}

	void save_image() {
		if (txcounter <= 43) {
			Serial.println("DATA SHORTAGE ERROR.");
		} else {
			unsigned int data;
			char fname[20];
			fname[16] = 0;
			for (int i = 0; i < 16; ++i) {
				data = ssram.read(PC1500Recorder::PROGNAME + i);
				fname[i] = data;
				if (data == 0)
					break;
			}
			if (strlen(fname) == 0) {
				strcpy(fname, "TEMPPC1");
			} else if (strlen(fname) > 8) {
				fname[8] = 0;
			}
			strcat(fname, ".REC");
			Serial.print("prog name = ");
			Serial.println(fname);
			sdfile = SD.open(fname, FILE_WRITE);

			// if the file is available, write to it:
			if ( sdfile ) {
				// skip first 0xA
				for (int i = 1; i < txcounter; ++i) {
					data = ssram.read(i);
					sdfile.write((byte)data);
				}
				sdfile.close();
				Serial.println("saved.");
			}
			// if the file isn't open, pop up an error:
			else {
				Serial.print("error opening '");
				Serial.print(fname);
				Serial.println("'");
			}
		}
	}

	void xout(long bytes) {
		unsigned int length = bytes;
		int i;

		//length = 0x2a + ((ssram.read(0x24)<<8) + ssram.read(0x25)) + 4;

		for (i = 0; i < 10196 /* 20392*/; ++i) {
			pulseOut(PULSE_PC15_1);
		}
		quadOut(0x0A);
		for (i = 0; i < 42; ++i) {
			byteOut(ssram.read(i));
		}
		// long stop bits 632 ~ 640
		for (i = 0; i < 640 - 48; ++i) {
			pulseOut(PULSE_PC15_1);
		}
		for (i = 42; i < length - 1; ++i) {
			byteOut(ssram.read(i));
		}
		// long stop bits 600 ~ 640
		for (i = 0; i < 640 - 48; ++i) {
			pulseOut(PULSE_PC15_1);
		}
		byteOut(ssram.read(length-1));
		// long stop bits 618 ~ 623
		for (i = 0; i < 624 - 48; ++i) {
			pulseOut(PULSE_PC15_1);
		}

	}

};

long load_file(const char * fname) {
	enum TYPE {
		REC,
		IMG,
		BAS,
	} type = REC;
	Serial.println(fname + strlen(fname));
	if ( strcmp(fname + strlen(fname) - 4,".IMG") == 0 ) {
		type = IMG;
	}
	long length = 0;
	long offset = 0;

	if ( type == IMG ) {
		offset = 42;
	}

	sdfile = SD.open(fname, FILE_READ);
	while ( sdfile.available() ) {
		int d = sdfile.read();
		ssram.write(offset + length, (unsigned char) d );
		length++;
	}
	sdfile.close();

	int i, sum = 0;
	byte t;
	if ( type == IMG ) {
		//
		Serial.println("IMG");
		Serial.println(length, HEX);
		/*
		for(i = offset; i < offset+length; ++i) {
			if ( (i&0x0F) == 0 ) {
				Serial.println();
				Serial.print(i>>4&0x0f, HEX);
				Serial.print(i&0x0f, HEX);
				Serial.print(": ");
			}
			t = ssram[i];
			Serial.print(t>>4&0x0f, HEX);
			Serial.print(t&0x0f, HEX);
			Serial.print(" ");
		}
		Serial.println();
		*/
		//
		for(i = 0; i < 8; i++) {
			ssram.write(i, 0x10+i);
		}
		ssram.write(0x08,0x01);
		for(i = 0x09 ; i < 0x28; ++i) {
			ssram.write(i,0);
		}
		for(i = 0; i < min(strlen(fname), 16); ++i) {
			if ( fname[i] == '.' || fname[i] == 0 )
				break;
			ssram.write(0x09 + i, fname[i]);
		}
		ssram.write(0x22, 0x40);
		ssram.write(0x23, 0xc5);
		ssram.write(0x24, (length+1)>>8 & 0xff); // + one for the last 0xff
		ssram.write(0x25, (length+1) & 0xff);

		for(i = 0; i < 40; ++i) {
			sum += ssram.read(i);
		}
		ssram.write(0x28, sum>>8 & 0xff);
		ssram.write(0x29, sum & 0xff);

		sum = 0;
		for( i = 0; i < length; ++i) {
			t = ssram[offset+i];
			if ( t == 0xff )
				break;
			sum += ssram[offset+i];
		}
		length = i;

		ssram.write(offset+length, 0xff);
		sum += 0xff;

		ssram.write(offset+length+1, sum>>8 & 0xff);
		ssram.write(offset+length+2, sum & 0xff);
		ssram.write(offset+length+3, 0x55);

		for(i = 0; i < offset+length+4; ++i) {
			if ( (i&0x0F) == 0 ) {
				Serial.println();
				//Serial.print(i>>4&0x0f, HEX);
				//Serial.print(i&0x0f, HEX);
				//Serial.print(": ");
			}
			t = ssram[i];
			Serial.print(t>>4&0x0f, HEX);
			Serial.print(t&0x0f, HEX);
			Serial.print(" ");
		}
		Serial.println();
		return length+offset+4;
	}
	return length;
}

void files(const char * path) {
	char filename[16];
	unsigned int len, i;
	File dir = SD.open(path);
	while (true) {
		File entry =  dir.openNextFile();
		if (! entry) {
		  // no more files
		  break;
		}
		strcpy(filename, entry.name());
		len = strlen(filename);
		// neglect invisible files
		if ( ( filename[0] != '.' && filename[0] != '_' ) &&
			( strcmp(filename+len-4,".REC") == 0 ||
				strcmp(filename+len-4, ".IMG") == 0 ||
				strcmp(filename+len-4,".BAS") == 0 ||
				entry.isDirectory() ) ) {
			Serial.print(filename);
			if (entry.isDirectory()) {
				Serial.println("/");
			} else {
				// files have sizes, directories do not
				for(i = len; i < 16; ++i) {
					Serial.print(' ');
				}
				Serial.println(entry.size(), DEC);
			}
		}
		entry.close();
	}
	dir.close();
}



E500Recorder e500;
PC1500Recorder pc1500;


void setup() {
  // put your setup code here, to run once:

  // SPI start
  pinMode(SRAM_CS_PC6, OUTPUT);
  digitalWrite(SRAM_CS_PC6, HIGH);
  pinMode(SRAM_HOLD_PF7, OUTPUT);
  digitalWrite(SRAM_HOLD_PF7, HIGH);
  pinMode(SD_CS_PF5, OUTPUT);
  digitalWrite(SD_CS_PF5, HIGH);
  SPI.begin();

  pinMode(XOUT_PD0,INPUT);
  pinMode(XIN_PD1,OUTPUT);
  digitalWrite(XIN_PD1, LOW);
  pinMode(PIN13_PC7,OUTPUT);
  digitalWrite(XIN_PD1, LOW);

  Serial.begin(38400);
  while (!Serial) {}
  Serial.println("Started.");

  if ( ssram.begin() ) {
    Serial.println("SPI SRAM started.");
  }
  // see if the card is present and can be initialized:
  //digitalWrite(SD_CS,LOW);
  if (SD.begin(SD_CS_PF5)) {
    Serial.println("SD Card started.");
  } else {
    Serial.println("SD Card start failed.");
  }
  //digitalWrite(SD_CS,HIGH);

  mode = MODE_IDLE;
}

void loop() {
	char ch, buf[32];
	long n = 0;
	if ( Serial.available() > 0 ) {
		buf[0] = 0;
		while (Serial.available() > 0 && n < 28 ) {
			ch = Serial.read();
			if ( isspace(ch) ) {
				break;
			}
			buf[n] = ch;
			n++;
			buf[n] = 0;
			if ( Serial.available() == 0 )
				delay(5);
		}
		strupr(buf);

		switch(buf[0]) {
		case 'S':
			pinMode(XIN_PD1,INPUT);
			break;
		case 'A':
			pinMode(XIN_PD1,OUTPUT);
			break;
		case 'F': // Files
			files("/"); //root);
			Serial.println("Ready.");
			Serial.println();
			break;
		case 'P': // Play
			//strcat(buf+1,".REC");
			Serial.print("play '");
			Serial.print(buf+1);
			Serial.print("' start...");
			n = load_file(buf+1);
			pc1500.xout(n);
			Serial.println("finished.");
			mode = MODE_IDLE;
			break;
		case 'R': // Record
			mode = MODE_IDLE;
			Serial.println("Listening...");
			while (mode != MODE_STOP) {
				  int pulse = getPulse();
				  if ( mode == MODE_IDLE ) {
					  if ( pulse == PULSE_E500_0 ) {
						  mode = MODE_E500;
					  } else if ( pulse == PULSE_PC15_1 ) {
						  mode = MODE_PC1500;
					  }
				  }

				  if ( mode == MODE_E500 ) {
					  e500.xin_pulses(pulse);
					  if ( e500.state == e500.END || e500.state == e500.ERR )
						  mode = MODE_STOP;

				  } else if ( mode == MODE_PC1500 ){
					  pc1500.xin_pulses(pulse);
					  if ( pc1500.read_finished() ) {
						  pc1500.save_image();
						  pc1500.reset();
						  mode = MODE_STOP;
					  }
				  }
			}
			Serial.println("Done.");
			break;
		default:
			break;
		}
	}
}

