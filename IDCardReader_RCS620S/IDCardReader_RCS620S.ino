#include <ctype.h>

#include <Wire.h>
#include "RTC.h"
#include "IOExpander.h"
#include "MCP23009.h"
#include "CharacterLCD.h"
#include "MCP2300xLCD.h"

#include "RCS620S.h"
#include "FCF.h"
#include "Tools.h"

void print_FCF(Print & prn, FCF_IDCard & idcard);

#define CHARNUM(x)  ((x)? ((x) - '0') & 0x0f : 0)

RCS620S nfcreader(Serial1);

#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 333
#define DETECT_INTERVAL 1000

RTC rtc((uint8_t)RTC::MAXIM_DS3234);

MCP2300xLCD lcd(0x00, 1, 0xff, 2, 3, 4, 5, 6, 7); // for adafruit i2c/spi lcd adapter
bool lcdbacklight_On = false;
#define LED13 13

long lastPolling = 0, lastDetect = 0;
static uint8_t payload[] = { 
  0x00, 0xff, 0xff, 0x01, 0x00   };
FCF_IDCard idcard;
  uint8_t tmp[64];
  char tmpstr[16];

void FCF_print();
uint16_t FCF_Read();
uint16_t KID_Read();
void print_Timestamp(Print & prn);

int roundrobin = 0;

void setup(void) {
  int16_t c;

  Serial.begin(57600);
  Serial << "Hello." << endl;

  Serial1.begin(115200);	
  if ( !nfcreader.start() ) {
    Serial << "RCS620S start failed." << endl;
    while (1);
  }
  
  Wire.begin();
  rtc.begin();
  lcd.begin(20,4);
  
  pinMode(LED13, OUTPUT);	

}

void loop() {
  uint16_t reslen;

  while(1) {

    if ( lastDetect + DETECT_INTERVAL < millis() &&
      lastPolling + POLLING_INTERVAL < millis() ) {
      digitalWrite(LED13, HIGH);
      nfcreader.timeout = COMMAND_TIMEOUT;  
      *((uint16_t*) &payload[1]) = NFC::FELICA_SYSCODE_COMMON;
      roundrobin = (roundrobin+1) % 2;
      if( (roundrobin == 0) && (reslen = nfcreader.InListPassiveTarget(1, NFC::BAUDTYPE_212K_F, payload, tmp)) ) {
        lastDetect = millis();
        lcd.clear(); lcd.backlightOn(); lcdbacklight_On = true;
        idcard.type = FCF_IDCard::FCF;
        memcpy(idcard.id, nfcreader.idm, 8);
//        Serial << endl << "IDm: ";
//        printBytes(Serial, nfcreader.idm, 8);
//        Serial << "." << endl;
//        Serial << "PMm: ";
//        printBytes(Serial, nfcreader.pmm, 8);
//        Serial << "." << endl;
        //
        rtc.update();
        print_Timestamp(Serial); Serial.print(' ');
        lcd.setCursor(0,0); print_Timestamp(lcd);
        if ( idcard.type == FCF_IDCard::FCF )
          printBytes(Serial, idcard.id, 8, '-');
        else
          printBytes(Serial, idcard.id, idcard.id[0]+1, '-');
        Serial.println();
        if ( FCF_Read() ) {
          Serial.print((const char*)"FC ");
          print_FCF(Serial, idcard); Serial.println();
          lcd.setCursor(0,2);print_FCF(lcd, idcard);
        } else {
          Serial.print((const char *)"Unknown FeliCa, ");
        }
        Serial.println();
      } 
      else if ( (roundrobin == 1) && (reslen = nfcreader.InListPassiveTarget(1, NFC::BAUDTYPE_106K_A, payload, tmp)) != 0 ) {
        lastDetect = millis();
        lcd.clear(); lcd.backlightOn(); lcdbacklight_On = true;
        idcard.type = FCF_IDCard::MFID;
        memcpy(idcard.id, nfcreader.idm, nfcreader.idm[0]+1);
        //printBytes(Serial, nfcreader.idm, nfcreader.idm[0]+1);
        //Serial.println();
        rtc.update();
        print_Timestamp(Serial); Serial.print(' ');
        lcd.setCursor(0,0); print_Timestamp(lcd);
        if ( idcard.type == FCF_IDCard::FCF )
          printBytes(Serial, idcard.id, 8, '-');
        else
          printBytes(Serial, idcard.id, idcard.id[0]+1, '-');
        Serial.println();
        if ( KID_Read() ) {
          Serial.print((const char*)"MF ");
          print_FCF(Serial, idcard); Serial.println();
          lcd.setCursor(0,2); print_FCF(lcd, idcard);
        } else {
          Serial.print((const char *)"Unknown Mifare, ");
        }
        Serial.println();        
      } 
      nfcreader.RFOff();
      digitalWrite(LED13, LOW);
      lastPolling = millis();
    } else {
      if (  lcdbacklight_On && lastDetect + 5000 < millis() ) {
        lcd.backlightOff();
        lcd.clear();
        lcdbacklight_On = false;
      }
    }
  }
}

void print_Timestamp(Print & prn) {
  prn << "20" << (rtc.date>>20&0x0f) << (rtc.date>>16&0x0f) << '/' 
        << (rtc.date>>12&0x0f) << (rtc.date>>8&0x0f) << '/'
        << (rtc.date>>4&0x0f) << (rtc.date>>0&0x0f);
  prn.print("-");
  prn << (rtc.time>>20&0x0f) << (rtc.time>>16&0x0f) << ':'
        << (rtc.time>>12&0x0f) << (rtc.time>>8&0x0f) << ':'
        << (rtc.time>>4&0x0f) << (rtc.time>>0&0x0f);
}

void print_FCF(Print & prn, FCF_IDCard & idcard) {
  char tmpstr[16];
  prn.print('[');
  prn.print(idcard.division, HEX);
  memcpy(tmpstr, idcard.pid, 12);
  tmpstr[12] = 0;
  prn.print("] ");
  prn.print(idcard.pid);
  prn.print('-');
  prn.print(idcard.issue);
  prn.print(' ');
  prn.print( idcard.gender() );
  /*
  prn.print(", ORG. ");
  prn.print( idcard.orgid );
  prn.print(", DOFISSUE ");
  prn.print( idcard.doi, HEX );
  */
  prn.print(" EXP.");
  prn.print( idcard.gdthru, HEX );
}


uint16_t FCF_Read() {
  uint16_t len;
  
  len = nfcreader.FeliCa_RequestService(NFC::FELICA_SERVICE_FCF);
//  Serial.print("Service Key version: ");
//  Serial.println(len, HEX);
  if ( len == 0xffff ) 
    return 0;
    
//  Serial.print("FCF ID Card: ");
  if ( ! nfcreader.FeliCa_ReadWithoutEncryption(NFC::FELICA_SERVICE_FCF, 0, tmp ) ) 
    return 0;

  idcard.division = CHARNUM(tmp[0])<<4 | CHARNUM(tmp[1]);
  memcpy(idcard.pid, tmp+2, 12);
  idcard.issue = tmp[14] - '0';
  idcard.gend = tmp[15] - '0';

  if ( ! nfcreader.FeliCa_ReadWithoutEncryption(NFC::FELICA_SERVICE_FCF, 2, tmp ) ) 
    return 0;
  memcpy(tmpstr, tmp, 8);
  tmpstr[8] = 0;
  idcard.orgid = atol(tmpstr);
  memcpy(tmpstr, tmp+8, 8);
  tmpstr[8] = 0;
  idcard.doi = strtol(tmpstr, 0, 16);            

  if ( ! nfcreader.FeliCa_ReadWithoutEncryption(NFC::FELICA_SERVICE_FCF, 3, tmp ) ) 
    return 0;
  memcpy(tmpstr, tmp, 8);
  tmpstr[8] = 0;
  idcard.gdthru = strtol(tmpstr, 0, 16);
    
  return 48;
}

uint16_t KID_Read() {
  uint32_t t, gthru;
  uint16_t len;
  if ( ! nfcreader.Mifare_BlockAuth(0x04, (const uint8_t *)"A\xff\xff\xff\xff\xff\xff") ) 
    return 0;
  len = nfcreader.Mifare_Read((const uint8_t)0x04, (uint8_t *) tmp);
  if ( ! len )
    return 0;
  
  idcard.division = tmp[1];
  memcpy(idcard.pid, tmp+2, 8);
  idcard.issue = tmp[10] - '0';

  len = nfcreader.Mifare_Read((const uint8_t)0x06, (uint8_t *) tmp);
  if ( ! len )
    return 0;
 
  idcard.gend = tmp[7] - '0';
  memcpy(tmpstr, tmp+8, 7);
  tmpstr[7] = 0;
  // assume tmpstr[0] == 'H'
  t = strtol(tmpstr+1, 0, 16);
  idcard.doi = t & 0xffff;
  idcard.gdthru = idcard.doi;
  t >>= 16;
  t = ((t>>4)&0x0f)*10 + t&0x0f;
  t += 1989;
  gthru = t + 1;
  idcard.doi |= ((t/1000)%10) <<28 | ((t/100)%10)<<24 | ((t/10)%10)<<20 | (t%10)<<16 ;
  t = gthru;
  idcard.gdthru |= ((t/1000)%10) <<28 | ((t/100)%10)<<24 | ((t/10)%10)<<20 | (t%10)<<16 ;

  idcard.orgid = 40010071; // fixed const value.

  return 1;
}

