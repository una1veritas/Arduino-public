
/*
 * Da Vinci 32U pin assign by Arduino Micro Bootloader
 * From USB connecter, counter clockwise
 * 1. GND
 * 2. Vcc 5V
 * 3. Reset
 * 4. D3 		PD0 SCL
 * 5. D2  		PD1 SDA
 * 6. D0  		PD2 RxD1
 * 7. D1  		PD3 TxD1
 * 8. D4 		PD4
 * 9. NA  		PD5 ~CTS (Tx LED)
 * 10. D12/A11 	PD6
 * 11. D6/A7	PD7
 * 12. D5   	PC6
 * 13. D13 (LED) PC7
 * 14. D23/A5	PF0
 * 15. D22/A4 	PF1
 * 16.    		PE2 ~HWB
 *
 * 17. GND
 * 18. D21/A3 		PF4
 * 19. D20/A2		PF5
 * 20. D19/A1		PF6
 * 21. D18/A0  		PF7
 * 22. D17			PB0 SS
 * 23. D15 			PB1 SCLK
 * 24. D16 			PB2 MOSI
 * 25. D14 			PB3 MISO
 * 26. D8/A8 		PB4
 * 27. D9/A9 		PB5
 * 28. D10/A10 		PB6
 * 29. D11 			PB7 ~RTS
 * 30. D7 			PE6
 * 31. Vref
 * 32. GND
 */

#include <string.h>

#include <SPI.h>
#include "SdFat.h"
#include "SPISRAM.h"

#include "DataRecorder.h"

// sharp 11 pin interface
const int PF7_RTS = 18; 	// <-- 4 BUSY/RTS (OUT)
const int PF4_DOUT = 21; 	// <-- 5 DOUT/DSR(NOY USE) (OUT)
const int PB6_CASOUT = 10; 	// --> 6 XIN/RxD (IN)
const int PB7_CASIN = 11;  	// <-- 7 XOUT/TxD (OUT) PocketPC/Interface
const int PF1_CD = 22;  	// --> 8 DIN/CD (IN) <--> DTR(ER)
const int PF0_CTS = 23;		// --> 9 ACK/CS CTS (IN)
const int PC6_DTR = 5;		// <-- 10 EX1/ER DTR (OUT)
const int PD7_EX2 = 6; 		// --> 11 EX2/CI Calling Indicator (IN)

const int PF5_SD_CS = 20;
const int PC6_SRAM_CS = 17;
const int PF7_SRAM_HOLD = 18;
const int PC7_LED = 13;		// <-- 10 EX1/ER DTR (OUT)

SPISRAM sram(PC6_SRAM_CS, SPISRAM::BUS_MBits); // CS pin
SdFat microSD;
File thefile;


DataRecorder recorder(PB7_CASIN, PB6_CASOUT, sram);

void setup() {
  // put your setup code here, to run once:

	Serial.begin(115200);
	while (!Serial) {}

	// SPI start
	pinMode(PC6_SRAM_CS, OUTPUT);
	digitalWrite(PC6_SRAM_CS, HIGH);
	pinMode(PF7_SRAM_HOLD, INPUT);
	pinMode(PF5_SD_CS, OUTPUT);
	digitalWrite(PF5_SD_CS, HIGH);
	SPI.begin();
	// SPI start end

	pinMode(PB6_CASOUT,INPUT); // high imp. in inactive mode
	pinMode(PB7_CASIN,INPUT);
	pinMode(PC7_LED,OUTPUT);

	// see if the card is present and can be initialized:
	digitalWrite(PC7_LED, HIGH);
	Serial.print(F("micro SD Card "));
	while ( !microSD.begin(PF5_SD_CS, SD_SCK_MHZ(16)) ) {
		digitalWrite(PC7_LED,HIGH);
		delay(1000);
		digitalWrite(PC7_LED, LOW);
		delay(1000);
	}
	Serial.println(F("started."));
	digitalWrite(PC7_LED, LOW);

	digitalWrite(PC7_LED, HIGH);
	Serial.print(F("SPI SRAM "));
	while ( !sram.begin() ) {
		digitalWrite(PC7_LED,HIGH);
		delay(1000);
		digitalWrite(PC7_LED, LOW);
		delay(1000);
	}
	Serial.println(F("started."));
	digitalWrite(PC7_LED, LOW);

}

void loop() {
	// put your main code here, to run repeatedly:
	char ch;
	char cmdline[24];
	long cmdlen;

	unsigned int iu;
	char filename[18];
	long tmilli;

	// get command string into buf
	if (Serial.available() > 0) {
		cmdlen = 0;
		while ( Serial.available() > 0 ) {
			cmdline[cmdlen] = toupper(Serial.read());
			if ( isspace(cmdline[cmdlen]) )
				break;
			cmdlen++;
		}
		cmdline[cmdlen] = 0;
		while ( Serial.available() > 0 )
			Serial.read();
		Serial.println(cmdline);

		// perform command
		if ( cmdline[0] == 'R' ) {
			Serial.println(F("Record"));
			record(cmdline+1);
		} else if ( cmdline[0] == 'P' ) {
			Serial.println(F("Play"));
			Serial.print(F("file: "));
			Serial.print(cmdline+1);
			Serial.println();
			thefile = microSD.open(cmdline+1, FILE_READ);
			if ( thefile ) {
				recorder.fileIn(thefile);
				Serial.println(F(" loaded, playing..."));
				recorder.play();
				Serial.println(F("done."));
			} else {
				Serial.println(" open failed.");
			}
		} else if ( cmdline[0] == 'D' ) {
			Serial.println(F("Files:"));
			list_files();
			Serial.println();
		} else if ( cmdline[0] == 'C' ) {
			Serial.print(F("Change Dir to ")); Serial.println(cmdline+1);
			if ( !microSD.chdir(cmdline+1) ) {
				Serial.println(F("Failed."));
			}
		}
	}
}

void record(char * filename) {
	Serial.print(F("Recording..."));
	recorder.record();
	if (recorder.record_completed()) {
		Serial.println(F(" completed."));
		if ( strlen(filename) == 0 ) {
			filename = recorder.imageName();
		}
		if (strlen(filename) == 0) {
			Serial.println(F("Canceled, empty name."));
			return;
		}
		Serial.print(F("Flie: '"));
		Serial.print(filename);
		Serial.print("'");
		thefile = microSD.open(filename, FILE_WRITE);
		if (!thefile) {
			Serial.print(F(" open failed."));
		} else {
			if ( recorder.imageType() == recorder.DATA_E500_CAS) {
				recorder.fileOut(thefile, 0x32);
			} else {
				recorder.fileOut(thefile);
			}
			Serial.println(F(" saved."));
		}
		thefile.close();
	}
}

void list_files() {
	char buf[32];
	long nsize;

	microSD.vwd()->rewind();
	while (thefile.openNext(microSD.vwd(), O_READ)) {
	    thefile.getName(buf,32);
	    if ( buf[0] == '.' || buf[0] == '_' ) {
	    	thefile.close();
	    	continue;
	    }
		if (thefile.isDir()) {
			// Indicate a directory.
			strcat(buf, "/");
			Serial.println(buf);
		} else {
			nsize = Serial.print(buf);
			for(int i = 0; i < 18 - nsize; i++)
				Serial.print(' ');
			nsize = thefile.available();
			if ( nsize < 1024 ) {
				nsize = Serial.print(nsize);
			} else {
				nsize = Serial.print( (nsize/1024.0), 1 );
				nsize += Serial.print('K');
			}
			for(int i = 0; i < 5 - nsize; i++)
				Serial.print(' ');
			Serial.print(' ');
			buf[0] = thefile.read();
			switch( (unsigned char) buf[0] ) {
			case recorder.DATA_E500_CSAVE_M:
				Serial.print(F("PC-E500 CSAVE M"));
				break;
			case recorder.DATA_E500_CSAVE:
				Serial.print(F("PC-E500 CSAVE"));
				break;
			case recorder.DATA_E500_CAS:
				Serial.print(F("PC-E500 ASCII"));
				break;
			case recorder.DATA_PC150x_BASIC_IMG:
				Serial.print(F("PC-150x BASIC IMG"));
				break;
			case recorder.DATA_E500_DISK_BIN:
				Serial.print(F("PC-E500 DISK BIN"));
				break;
			default:
				Serial.print(F("0x"));
				Serial.print(buf[0]>>4&0x0f, HEX);
				Serial.print(buf[0]&0x0f, HEX);
				break;
			}
			Serial.println();
	    }
	    thefile.close();
	}
	Serial.println();
}
