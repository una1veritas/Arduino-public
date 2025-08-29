#include <Wire.h>
#include "PN532_I2C.h"
#include "ISO14443.h"
#include "StreamUtils.h"

const int IRQ = 2;
const int RST = 0xff;  // Not connected by default on the NFC Shield
// tied with CPU RESET

PN532 nfc(PN532::I2C_ADDRESS, IRQ, RST);
byte buff[80];
uint32_t lastDetect = 0;
ISO14443Card card, lastcard;

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B
};
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const byte SinShimo_2[] = {
  0x31, 0x53, 0x38, 0x32, 0x35, 0x34, 0x31, 0x38, 0x35, 0x34, 0x32, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x89, 0xBA, 0x89, 0x92, 0x81, 0x40, 0x90, 0x5E, 0x88, 0xEA, 0x81, 0x40, 0x81, 0x40, 0x81, 0x40,
  0x53, 0x34, 0x31, 0x30, 0x34, 0x30, 0x37, 0x31, 0x48, 0x31, 0x35, 0x30, 0x37, 0x30, 0x31, 0x33,
};

enum MODE {
  MODE_READ = 0,
  MODE_WRITE = 1,
  MODE_KEYB_WRITE = 2,
} mode;

void setup() {
  byte cnt;

  Wire.begin();
  nfc.begin();

  Serial.begin(9600);
  Serial.println("PN532 via I2C Firmware inspection/FeliCa read test.");

  if ( nfc.GetFirmwareVersion()
       && nfc.getCommandResponse(buff) ) {
    Serial.print("IC version PN53'");
    Serial.print((char) buff[0]);
    Serial.print("', firmware ver. ");
    Serial.print(buff[1]);
    Serial.print(" rev. ");
    Serial.print(buff[2]);
    Serial.print(", supports ");
    Serial.print((buff[3] & 0b001 ? "Type A, " : ""));
    Serial.print((buff[3] & 0b010 ? "Type B, " : ""));
    Serial.print((buff[3] & 0b001 ? "ISO 18092 " : ""));
    Serial.println(".");
  }
  else {
    Serial.println("PN532 not found.");
    while (1);
  }

  Serial.println("  SAMConfiguration ");
  if ( nfc.SAMConfiguration() && (0 == nfc.getCommandResponse(buff)) ) {
    Serial.println();
  }
  else {
    Serial.println(" failed.");
  }
  if ( nfc.GetGeneralStatus() && (cnt = nfc.getCommandResponse(buff)) ) {
    for (int i = 0; i < cnt; i++) {
      Serial.print(buff[i] >> 4, HEX);
      Serial.print(buff[i] & 0x0f, HEX);
      Serial.print(' ');
    }
    Serial.println();
    Serial.println("  GetGeneralState >>");
  }
  nfc.PowerDown(0xff);
  Serial.print(nfc.getCommandResponse(buff));
  Serial.println("  PowerDown >>");

  mode = MODE_READ;
}

void loop() {
  byte c;
  byte polling[] = {
    2,
    NFC::BAUDTYPE_106K_A,
    NFC::BAUDTYPE_212K_F
  };
  char tmp[16];

  if ( (millis() > lastDetect + 1000) and
       (c = nfc.InAutoPoll(1, 1, polling + 1, polling[0])) and
       (c = nfc.getAutoPollResponse()) ) {
    //mon << mon.printArray(tmp, 8) << mon.endl;
    // NbTg, type1, length1, [Tg, ...]
    card = nfc.target;
    Serial << "Card type: " << (int) card.type << NL;
    if ( card != lastcard ) {
      lastcard = card;
      lastDetect = millis();
      if ( card.type == NFC::CARDTYPE_MIFARE ) {
        //tone(4, 1800, 100);
        Serial.println("Mifare: ");
        printBytes(Serial, card.ID, card.IDLength);
        Serial << NL;
        nfc.targetSet(0x10, card.ID, card.IDLength);
        if ( nfc.mifare_AuthenticateBlock(4, factory_a) //IizukaKey_b)
             && nfc.mifare_ReadBlock(4, buff) ) {
          Serial.println("Auth Succeeded.");
          if ( mode == MODE_READ ) {
            if ( nfc.mifare_ReadBlock(5, buff+16) && nfc.mifare_ReadBlock(6, buff+32) ) {
              for(int b = 0; b < 3; b++) {
                printBytes(Serial, buff+(16*b), 16);
                Serial.println();
                for (int i = 0; i < 16; i++) {
                  if ( isprint(buff[16*b+i]) ) {
                    Serial.print((char)buff[16*b+i]);
                    Serial.print(' ');
                  } else {
                    Serial.print("  ");
                  }
                  Serial.print(' ');
                }
                Serial.println();
              }
              Serial.println();
            }
          }
          else if ( mode == MODE_WRITE ) {
            Serial << "Write mode. " << NL;
            memcpy(buff, SinShimo_2, 48);
            nfc.mifare_WriteBlock(4, buff);
            nfc.mifare_WriteBlock(5, buff+16);
            nfc.mifare_WriteBlock(6, buff+32);

            mode = MODE_READ;
            Serial << "Return to read mode. " << NL;
          } else if ( mode == MODE_KEYB_WRITE ) {
            uint8_t data[16];
            uint32_t result;
            result = nfc.mifare_ReadAccessConditions(1, data);
            Serial << "Access cond. " << NL;
            Serial.print(result, HEX);
            Serial.println();
            printBytes(Serial, data, 16);
            Serial << NL;
            
            memcpy(data, factory_a+1, 6);
            memcpy(data+10, IizukaKey_b+1, 6);
            uint32_t acbits = 0x8f7;
            Serial << "Data " << NL;
            printBytes(Serial, data, 16);
            Serial << NL;
            
	    result = nfc.mifare_WriteAccessConditions(1, acbits, data, data+10);
            Serial << "Access cond. " << NL;
            printBytes(Serial, data, 16);
            Serial << NL;
            
            mode = MODE_READ;
          }
        }
        else {
          //tone(4, 1200, 100);
          Serial.println("Auth Failed.");
        }
        Serial.println();
      }
      else {
        Serial << "Not a Mifare card!" << NL;
      }
    }
  }
  else {
    if (millis() > lastDetect + 5000)
      lastcard.clear();
  }

  if ( readToken(Serial, tmp, 16, 500) ) {
    String token = (char*)tmp;
    if ( token == String("Write") ) {
      mode = MODE_WRITE;
      Serial << "Accepted Write command." << NL;
    } else if ( token == String("KeyB") ) {
      mode = MODE_KEYB_WRITE;
      Serial << "Accepted Key B Write command." << NL;
    }
  }

}



