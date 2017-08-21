#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>

#include "RTC.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

/*
#include "CharacterLCD.h"
#include "ST7032i.h"
*/
#include <SPI.h>
/*
#include <SPIFRAM.h>
*/
#include <SdFat.h>
//#include <SdFatUtil.h>  // define FreeRam()
#include <SdFile.h>

#include "StringBuffer.h"
#include "main.h"

#define LOGCLOCK
/*
#define LED_GREEN_ON() { digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_RED, LOW); lastLEDLit = millis(); }
#define LED_RED_ON() { digitalWrite(LED_RED, HIGH); digitalWrite(LED_GREEN, LOW); lastLEDLit = millis(); }
#define LED_OFF()    { digitalWrite(LED_RED, LOW); digitalWrite(LED_GREEN, LOW); }

SPIFRAM fram(CS_SPIFRAM, SPIFRAM::BUSWIDTH_FM25V10);
*/
IDLog logidtmp;

SdFat SD;
SdFile logfile;
boolean SD_mounted;
enum {
	LOG_HELLO = 0,
	LOG_CARD,
	LOG_UNKNOWN,
	LOG_CLOCK,
};

void writestream(Print & , byte);
bool writelog(byte);
//bool writeToFRAM(byte);
bool saveToSD();
uint16_t read_fram_count(const uint16_t & addr);

char name[] = "LOG0000.CSV";

PN532 nfc(PN532::I2C_ADDRESS, PN532_IRQ, PN532_RST);
RTC rtc(RTC::MAXIM_DS1307);
/*
ST7032i lcd;
*/
uint32_t lastMillis;
ISO14443Card lastCard;
uint32_t lasttime;
uint32_t lastupdate;
uint32_t lastLEDLit;

byte buff[64];
StringBuffer strbuff((char*)buff, 64);

const byte IizukaKey_b[] = { 0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = { 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

byte polling[] = { 2, NFC::BAUDTYPE_106K_A, NFC::BAUDTYPE_212K_F };
IDIizuka * idptr;

// call back for file timestamps
void dateTime(uint16_t* date, uint16_t* time) {

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(rtc.year(), rtc.month(), rtc.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(rtc.hour(), rtc.minute(), rtc.second());
}

void stampout(Print & prn, uint32_t d, uint32_t t) {
	prn << Hex(t>>16 & 0xff) << ':' << Hex(t>>8 & 0xff) << ':' << Hex(t & 0x0ff);
	prn << F(",20") << Hex(d>>16 & 0xff) << '/' << Hex(d>>8 & 0xff) << '/' << Hex(d);
}

boolean ID_valid(IDIizuka * ptr) {
	boolean result = true;
	if ( ptr == NULL )
		return false;
	result = result && (ptr->occup == 'S' || ptr->occup == 'G');
	for(int i = 0; i < 8; i++) {
		result = result && isdigit(ptr->ID[i]);
	}
	result = result && isdigit(ptr->issue);
	return result;
}


void setup() {
	byte n;
/*
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	LED_GREEN_ON();
*/	
	Wire.begin();
	rtc.begin();
	nfc.begin();
/*
	lcd.begin();
	lcd.setContrast(32);  // 32 ~ 36
	lcd.print(F("Hello."));
*/	
	rtc.update();
	Serial.begin(57600);
	Serial.setTimeout(500); // for input
	Serial.println(F("Hello. "));
	stampout(Serial, rtc.date, rtc.time);
	Serial.println();
	
	tone(BUZZ, 880, 100);	

	Serial << F("PN53x ");
	if (nfc.GetFirmwareVersion() && nfc.getCommandResponse(buff)) {
		Serial.print((char) buff[0]);
		Serial.print(F(", "));

		if (nfc.SAMConfiguration() && (0 == nfc.getCommandResponse(buff))) {
			Serial.print(F("SAMConfiguration, "));
		}
#ifdef DEBUG
		if (nfc.GetGeneralStatus() && (n = nfc.getCommandResponse(buff))) {
			Serial.print(F("GetGeneralState: "));
			printHexString(Serial, buff, n);
		}
#endif
		nfc.PowerDown(0xff);
		if ( nfc.getCommandResponse(buff) )
			Serial.print(F("PowerDown. "));
		Serial.println();
	} else {
		Serial.print(F(" failed."));
		while (1)
			;
	}
	Serial.flush();
/*	
	Serial.println(F("SPI bus."));
	SPI.begin();
*/	
	SdFile::dateTimeCallback(dateTime);
	// create a new file in root, the current working directory
	name[3] = rtc.month()/10 + '0';
	name[4] = rtc.month()%10 + '0';
	name[5] = rtc.day()/10 + '0';
	name[6] = rtc.day()%10 + '0';
	
	Serial.print(F("SD "));
	for(int i = 0; i < 3; i++) {
		if ( SD.begin(CS_SD, SPI_HALF_SPEED) ) {
			SD_mounted = true;
			Serial.println(F("card mounted."));
			break;
		}
		delay(1000);
	}
	if ( SD_mounted ) {
		saveToSD();
	} else {
		Serial.println(F("mount failed."));
		tone(440, BUZZ, 500);
//		uint16_t n = read_fram_count(0);
		if ( n > 0 ) {
/*			lcd.setCursor(0,0);
			lcd.print(n);
			lcd.print(" data");
			lcd.setCursor(0,1);
			lcd.print(" in RAM");
*/		} else {
/*			lcd.setCursor(0,0);
			lcd.print("No SD");
			lcd.setCursor(0,1);
			lcd.print("USE FRAM");			
*/		}
		for(int i = 0; i < 3; i++) {
//			LED_RED_ON();
			delay(500);
//			lcd.noDisplay();
//			LED_OFF();
			delay(500);
//			lcd.display();
		}
//		lcd.clear();
	}

	// initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
	//pinMode(SD_CARD_INSERT, INPUT);
//	LED_OFF();
	
	lastMillis = millis();
}

void loop() {
	byte c;

	if (millis() > lastMillis + 1000) {
		if ( (c = nfc.InAutoPoll(1, 1, polling + 1, polling[0]))
			and (c = nfc.getAutoPollResponse())) { // (byte*) buff)) ) {
//			LED_OFF();
			// NbTg, type1, length1, [Tg, ...]
			if (nfc.target != lastCard) {
				lastCard = nfc.target;
				lastMillis = millis();
				if (lastCard.type == NFC::CARDTYPE_MIFARE) {
					if (c = nfc.mifare_AuthenticateBlock(4, IizukaKey_b)
							&& nfc.mifare_ReadBlock(4, buff) 
							&& ID_valid((IDIizuka*)buff) ) {
						tone(BUZZ, 880, 100);
//						LED_GREEN_ON();
						delay(50);
						tone(BUZZ, 1175, 100);
						idptr = (IDIizuka*) buff;
						
						stampout(Serial, rtc.date, rtc.time);
						Serial << ',' << lastCard << ',';
						Serial.print(idptr->occup);
						Serial.print('-');
						Serial.print( Hex(idptr->ID,8) );
						Serial.print('-');
						Serial.print(idptr->issue);
						Serial.println();
						
/*						lcd.setCursor(0,0);
						lcd.print( Hex(idptr->ID,8) );
						lcd.setCursor(7,1);
						lcd.print(idptr->occup);
*/						writelog(LOG_CARD);
					} else {
//						LED_RED_ON();
						Serial.println(F("Auth-Read-failed"));
						tone(BUZZ, 784, 100);
						delay(200);
						tone(BUZZ, 784, 100);
						writelog(LOG_UNKNOWN);
/*						lcd.setCursor(0,0);
						lcd.print( F("ERROR   ") );
*/					}
				}
			}
			nfc.PowerDown(0xff); 
			nfc.getCommandResponse(buff);
		} else if (millis() > lastMillis + 5000) {
			lastCard.clear();
/*			lcd.setCursor(0,0); 
			lcd.print(F("        "));
			lcd.setCursor(7,1);
			lcd.print(' ');
			LED_OFF();
*/		}
	}
/*
	if ( lastLEDLit + 500 < millis() )
		LED_OFF();
*/
	if ( millis() > lastupdate + 99 ) {
		rtc.update();
		if ( lasttime != rtc.time ) {
/*			lcd.setCursor(0,1);
			lcd << Hex(rtc.time>>16&0xff);
			if ( rtc.time & 1 )
				lcd << ' ';
			else
				lcd << ':';
			lcd << Hex(rtc.time>>8&0xff);
*/			lasttime = rtc.time;

#ifdef LOGCLOCK
			if ( (rtc.time & 0xfff) == 0x000 )
				writelog(LOG_CLOCK);
#endif
		}
		lastupdate = millis();
	}
	
	if ( Serial.available() ) {
		strbuff.clear();
		while( Serial.peek() != -1 ) {
			c = Serial.read();
			if ( c == '\r' || c == '\n' ) 
				break;
			strbuff.write(c);
		}
		if ( strbuff.length() > 0 ) {
			char tmp[8];
			strbuff.getToken(tmp, 8);
			// Serial << '\'' << tmp << '\'' << NL;
			if ( strcmp(tmp, "TIME") == 0 ) {
				strbuff.getToken(tmp, 8);
				rtc.time = strtol(tmp, NULL, 16);
				if ( rtc.time != 0 )
					rtc.setTime(rtc.time);
				else {
					rtc.updateTime();
					Serial.println(rtc.time, HEX);
				}
			} else if ( strcmp(tmp, "DATE") == 0 ) {
				strbuff.getToken(tmp, 8);
				rtc.date = strtol(tmp, NULL, 16);
				if ( rtc.date != 0 )
					rtc.setCalendar(rtc.date);
				else {
					rtc.updateCalendar();
					Serial.println(rtc.date, HEX);
				}
			}
		}
	}

}



bool writelog(byte func) {
	byte xsum = 0;

	if ( SD_mounted ) {
		logfile.open(name, O_WRITE | O_APPEND | O_CREAT);
	}
	if ( SD_mounted && logfile.isOpen() ) {
		writestream(logfile, func);
		logfile.sync();
		logfile.close();
	} else if (func == LOG_CARD || func == LOG_UNKNOWN) {
		SD_mounted = false;
//		writeToFRAM();
		return false;
	}
	return true;
}

void writestream(Print & prn, byte func) {
	stampout(prn, rtc.date, rtc.time);
	switch(func) {
	case LOG_HELLO:
		prn << ',' << F("Hello.");
		break;
	case LOG_CARD:
		prn << ',' << F("RFID") << ',' << lastCard << ',' << (char) idptr->occup << '-';
		prn << Hex(idptr->ID,8) << '-' << (char)idptr->issue;
		break;
	case LOG_UNKNOWN:
		prn << ',' << F("RFID") << ',' << lastCard << ',' << F("Unknown");
		break;
	case LOG_CLOCK:
		prn << ',' << F("CLOCK");
		break;
	}
	prn << NL;
}

bool saveToSD() {
	uint16_t count;
	byte xsum = 0;

	Serial << F("Save to SD: ") << NL;
	logfile.open(name, O_WRITE | O_APPEND | O_CREAT);
	if ( !logfile.isOpen() ) 
		return false;
/*
	count = read_fram_count(0);
	while ( count > 0 ) {
		count--;
		fram.read(sizeof(count)+count*sizeof(IDLog), 
				(byte*) &logidtmp, sizeof(IDLog));
		idptr = &logidtmp.id;
		lastCard = logidtmp.card;
		rtc.time = logidtmp.time;
		rtc.date = logidtmp.date;
		writestream(logfile, LOG_CARD);
		logfile.sync();
		logfile.close();
		return true;
	} else {
		Serial.println(F("Open logfile failed."));
#ifdef USE_FRAM
		Serial.println(F("USE Fram."));
		byte xsum = 0;
		
		fram.read(0, (byte*) &count, sizeof(count));
		memcpy((void*)&(logidtmp.id), (void*)idptr, sizeof(IDLog));
		logidtmp.card = lastCard;
		logidtmp.date = rtc.date;
		logidtmp.time = rtc.time;
		for(int i = 0; i < sizeof(IDLog)-1; i++) {
			xsum ^= ((byte*) &logidtmp)[i];
		}
		logidtmp.xsum = xsum;
		fram.write(sizeof(count) + count*sizeof(IDLog), (byte*)&logidtmp, sizeof(IDLog));
		count++;
		fram.write(0, (byte*) & count, sizeof(count));
		
		return true;
#endif
	}
	return false;

		
		write_fram_count(0, count);
		Serial << F("   fram count = ") << count << NL;
	}	
*/
	logfile.close();
	return true;
}

