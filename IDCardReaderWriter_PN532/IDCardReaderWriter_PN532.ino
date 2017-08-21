#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>
#include "RTC.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "CharacterLCD.h"
#include "ST7032i.h"

#include <SPI.h>
//#include <SPIFRAM.h>
#include <SdFat.h>
//#include <SdFatUtil.h>  // define FreeRam()
#include <SdFile.h>
#include "StringBuffer.h"
#include "Tools.h"

#include "main.h"

#define LOGCLOCK

#define LED_GREEN_ON() { digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_RED, LOW); lastLEDLit = millis(); }
#define LED_RED_ON() { digitalWrite(LED_RED, HIGH); digitalWrite(LED_GREEN, LOW); lastLEDLit = millis(); }
#define LED_OFF()    { digitalWrite(LED_RED, LOW); digitalWrite(LED_GREEN, LOW); }

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
RTC rtc(RTC::CHIP_ST_M41T62);
ST7032i lcd;

uint32_t lastMillis;
ISO14443Card lastCard;
uint32_t lasttime;
uint32_t lastupdate;
uint32_t lastLEDLit;

byte buff[32];

const byte IizukaKey_b[] = { 
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = { 
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

byte polling[] = { 
  2, NFC::BAUDTYPE_106K_A, NFC::BAUDTYPE_212K_F };
IDIizuka idcard;

// call back for file timestamps
void dateTime(uint16_t* date, uint16_t* time) {

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(rtc.year(), rtc.month(), rtc.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(rtc.hour(), rtc.minute(), rtc.second());
}

void printTimeStamp(Print & prn, uint32_t d, uint32_t t) {
  printByte(prn, t>>16 & 0xff);
  prn << ':';
  printByte(prn, t>>8 & 0xff);
  prn << ':';
  printByte(prn,t & 0x0ff);
  prn << F(",20");
  printByte(prn,d>>16 & 0xff);
  prn << '/';
  printByte(prn,d>>8 & 0xff);
  prn << '/';
  printByte(prn,d&0xff);
}

void printASCII(Print& prn, uint8_t* buf, int a, int b) {
  for(int c = 0; c < a; c++) {
    prn.print((char)buf[c]);
  }
}

boolean ID_valid(IDIizuka * ptr, const char type) {
  boolean result = true;
  if ( type == 'M' ) {
    if ( ptr == NULL )
      return false;
    result = result && (ptr->occup == 'S' || ptr->occup == 'G');
    for(int i = 0; i < 8; i++) {
      result = result && isdigit(ptr->ID[i]);
    }
    result = result && isdigit(ptr->issue);
    return result;
  } 
  else if ( type == 'F' ) {

  }
  return false;
}


void setup() {
  byte n;

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  LED_GREEN_ON();

  Wire.begin();
  rtc.begin();
  nfc.begin();
  lcd.begin();
  lcd.setContrast(32);  // 32 ~ 36
  lcd.print(F("Hello."));

  rtc.update();
  Serial.begin(57600);
  Serial.setTimeout(500); // for input
  Serial.println(F("Hello. "));
  printTimeStamp(Serial, rtc.date, rtc.time);
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
  } 
  else {
    Serial.print(F(" failed."));
    while (1)
      ;
  }
  Serial.flush();

  Serial.println(F("SPI bus."));
  SPI.begin();

  SdFile::dateTimeCallback(dateTime);
  // create a new file in root, the current working directory
  name[3] = rtc.month()/10 + '0';
  name[4] = rtc.month()%10 + '0';
  name[5] = rtc.day()/10 + '0';
  name[6] = rtc.day()%10 + '0';

  Serial.print(F("SD "));
  for (int count = 0; !SD_mounted && count < 5; count++) {
    if ( SD.begin(CS_SD, SPI_HALF_SPEED) ) {
      SD_mounted = true;
      Serial.println(F("card mounted."));
    } 
    else {
      Serial.println(F("mount failed."));
      tone(BUZZ, 440, 500);
      lcd.setCursor(0,0);
      lcd.print("No SD");
      for(int i = 0; i < 3; i++) {
        LED_RED_ON();
        delay(500);
        lcd.noDisplay();
        LED_OFF();
        delay(500);
        lcd.display();
      }
    }
    delay(500);
  }

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  //pinMode(SD_CARD_INSERT, INPUT);
  LED_OFF();

  lastMillis = millis();
}

void loop() {
  byte c;
  boolean result;

  if (millis() > lastMillis + 1000) {
    if ( (c = nfc.InAutoPoll(1, 1, polling + 1, polling[0]))
      and (c = nfc.getAutoPollResponse())) { // (byte*) buff)) ) {
      LED_OFF();
      // NbTg, type1, length1, [Tg, ...]
      if (nfc.target != lastCard) {
        lastCard = nfc.target;
        lastMillis = millis();
        if (lastCard.type == NFC::CARDTYPE_MIFARE) 
          result = read_MifareID();
        else if (lastCard.type == NFC::CARDTYPE_FELICA_212K) {
#ifdef DEBUG_FELICA
          Serial.println(F("IDm: "));
          printBytes(Serial, lastCard.ID, 8);
          Serial.println();
#endif			        
          result = read_FeliCaID();
        }
        if ( result ) {
          tone(BUZZ, 880, 100);
          LED_GREEN_ON();
          delay(50);
          tone(BUZZ, 1175, 100);

          printTimeStamp(Serial, rtc.date, rtc.time);
          Serial << ',' << lastCard << ',';
          Serial.print(idcard.occup);
          Serial.print('-');
          printASCII(Serial,(uint8_t*)idcard.ID,8,0);
          Serial.print('-');
          Serial.print((char)idcard.issue);
          Serial.println();

          lcd.setCursor(0,0);
          printASCII(lcd,(uint8_t*)idcard.ID,8,0);
          lcd.setCursor(7,1);
          lcd.print((char)idcard.occup);
          writelog(LOG_CARD);
        } 
        else {
          LED_RED_ON();
          Serial.println(F("Auth-Read-failed"));
          tone(BUZZ, 784, 100);
          delay(200);
          tone(BUZZ, 784, 100);
          writelog(LOG_UNKNOWN);
          lcd.setCursor(0,0);
          lcd.print( F("ERROR   ") );
        }				
      }
      nfc.PowerDown(0xff); 
      nfc.getCommandResponse(buff);
    } 
    else if (millis() > lastMillis + 5000) {
      lastCard.clear();
      lcd.setCursor(0,0); 
      lcd.print(F("        "));
      lcd.setCursor(7,1);
      lcd.print(' ');
      LED_OFF();
    }
  }

  if ( lastLEDLit + 500 < millis() )
    LED_OFF();

  if ( millis() > lastupdate + 99 ) {
    rtc.update();
    if ( lasttime != rtc.time ) {
      lcd.setCursor(0,1);
      printByte(lcd,rtc.time>>16&0xff);
      if ( rtc.time & 1 )
        lcd << ' ';
      else
        lcd << ':';
      printByte(lcd,rtc.time>>8&0xff);
      lasttime = rtc.time;

#ifdef LOGCLOCK
      if ( (rtc.time & 0xffff) == 0 )
        writelog(LOG_CLOCK);
#endif
    }
    lastupdate = millis();
  }

  /*	
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
   	*/

}


#ifdef SD_WRITE

bool writelog(byte func) {
  byte xsum = 0;

  do {
    if ( SD_mounted ) {
      logfile.open(name, O_WRITE | O_APPEND | O_CREAT);
    } 
    else {
      if ( SD.begin(CS_SD, SPI_HALF_SPEED) ) 
        SD_mounted = true;
      continue;
    }
    if ( SD_mounted && logfile.isOpen() ) {
      writestream(logfile, func);
      logfile.sync();
      logfile.close();
      break;
    } 
    else {
      lcd.setCursor(0,1);
      lcd.print("No SD CARD");
      for(int i = 0; i < 3; i++) {
        delay(500);
        lcd.noDisplay();
        delay(500);
        lcd.display();
      }
      SD_mounted = false;
    }
  } 
  while (true);
  return true;
}

void writestream(Print & prn, byte func) {
  printTimeStamp(prn, rtc.date, rtc.time);
  switch(func) {
  case LOG_HELLO:
    prn << ',' << F("Hello.");
    break;
  case LOG_CARD:
    prn << ',' << F("RFID") << ',' << lastCard << ',' << (char) idcard.occup << '-';
    printASCII(prn, (uint8_t*)idcard.ID,8, 0);
    prn << '-' << (char)idcard.issue;
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

#else

bool writelog(byte func) { return true; }
void writestream(Print & prn, byte func) { }

#endif

boolean read_MifareID() {
  byte c;
  if (c = nfc.mifare_AuthenticateBlock(4, IizukaKey_b)
    && nfc.mifare_ReadBlock(4, buff) ) {
    memcpy(&idcard, buff, sizeof(idcard));

    return true;
  }
  return false;
}


boolean read_FeliCaID() {
  int resp;
  word scver;
  const byte SYSCODE_REQUESTED[] = { 
    0, 0xfe, 0x00, 1, 0   }; // for syscode 0x00FE

  resp = nfc.InListPassiveTarget(1, NFC::CARDTYPE_GENERICPASSIVE_212K_F, SYSCODE_REQUESTED, sizeof(SYSCODE_REQUESTED));
  if ( resp == 0 || (resp = nfc.getCommandResponse(buff)) == 0 ) {
    Serial.println(F("list target failed."));
    return false;
  }
#ifdef DEBUG_FELICA
  Serial.print(F("list target: "));
  printBytes(Serial, buff, resp);
  Serial.println();
#endif
  nfc.targetSet(NFC::CARDTYPE_FELICA_212K, buff+4, 8);  
  LED_GREEN_ON();

  /* 
   	two-byte service code in low-byte first.
   	Suica, Nimoca, etc. 0x090f system 0x0300
   	Edy service 0x170f (0x1317), system 0x00FE // 8280
   	FCF 1a8b
   	*/
  scver = nfc.felica_RequestService(NFC::FELICA_SERVICE_FCF);
#ifdef DEBUG_FELICA
  Serial.print(F("ServCode Rev.: "));
  Serial.println(scver, HEX);
#endif
  if ( scver == 0xffff ) {
    Serial.println(F("Error on SVC Rrev."));
    return false;
  }
  word blocks[1];
  blocks[0] = 0x02;
  resp = nfc.felica_ReadWithoutEncryption(buff, (word)0x1a8b, 1, blocks );
#ifdef DEBUG_FELICA
  Serial.println(resp);
  printASCII(Serial, buff, resp*16);
  Serial.println();		
#endif
  if ( resp == 0 || (memcmp(buff, "40010071", 8) != 0)) {
    Serial.println(F("Fac. ID read error."));
    return false;
  }
  blocks[0] = 0x00;
  resp = nfc.felica_ReadWithoutEncryption(buff, (word)0x1a8b, 1, blocks );
  if ( resp == 0 ) {
    Serial.println(F("Stu. ID read error."));
    return false;
  }

#ifdef DEBUG_FELICA
  printASCII(Serial, buff, 16);
  Serial.println();		
#endif
  idcard.occup = buff[0];
  idcard.div = buff[1];
  memcpy(idcard.ID,buff+2,8);
  idcard.issue = buff[14];	

  return true;
}

