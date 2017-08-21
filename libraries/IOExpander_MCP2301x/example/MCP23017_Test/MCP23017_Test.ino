#include <Wire.h>

#include <IOExpander.h>
#include <MCP2301x.h>

#include <Tools.h>

MCP2301x ioxt(4);
long lastwatch = 0;
byte lastScan = 0xff, lastDetect = 0xff, lastCode = 0;
boolean writeMode = false;

word keyPressed(const byte detect, const byte scan);

void setup() {
  Serial.begin(19200);

  Wire.begin();
  ioxt.begin();
  ioxt.write16(MCP2301x::IODIRA, MCP2301x::ALL_INPUT);
  ioxt.write16(MCP2301x::GPPUA, MCP2301x::ALL_SET);

  Serial.println("Hello.");
  lastwatch = millis();
}

void loop() {
  byte scan, detect;
  word keycode;
  long diff;

    scan = ioxt.gpioB();
    if ( scan != 0xff &&  scan != lastScan ) {
      lastScan = scan;
      if ( writeMode ) {
      } else {
        detect = ioxt.gpioA();
        if ( detect != 0xff ) {
          lastDetect = detect;
          //
          keycode = keyPressed(lastDetect, lastScan);
          Serial.print(millis());
          Serial.print(": ");
/*          printBits(Serial, &lastScan, 8);
          Serial.print(' ');
          printBits(Serial, &lastDetect, 8);
          Serial.print(" code = ");
*/          printByte(Serial, (uint8_t) keycode);
          Serial.println();
        }
      }
    }
}

word keyPressed(const byte detect, const byte scan) {
  word code = 0;
  byte val;
  val = ~scan;
  switch (val) {
    case 0x01:
      code = 0 << 4;
      break;
    case 0x02:
      code = 1 << 4;
      break;
    case 0x04:
      code = 2 << 4;
      break;
    case 0x08:
      code = 3 << 4;
      break;
    case 0x10:
      code = 4 << 4;
      break;
    case 0x20:
      code = 5 << 4;
      break;
    case 0x40:
      code = 6 << 4;
      break;
    case 0x80:
      code = 7 << 4;
      break;
    default:
      code = 8 << 4;
      break;
  }
  val = ~detect;
  switch (val) {
    case 1:
      code |= 0;
      break;
    case 2:
      code |= 1;
      break;
    case 4:
      code |= 2;
      break;
    case 8:
      code |= 3;
      break;
    case 16:
      code |= 4;
      break;
    case 32:
      code |= 5;
      break;
    case 64:
      code |= 6;
      break;
    case 128:
      code |= 7;
      break;
    default:
      code |= 8;
      break;
  }
  return code;
}

