#include <Wire.h>

#include <IOExpander.h>
#include <MCP2301x.h>

#include <Tools.h>

MCP2301x ioxt(4);
long lastwatch = 0;
byte lastScan = 0xff, lastDetect = 0xff, lastCode = 0;
boolean writeMode = false;

void setup() {
  Serial.begin(19200);

  Wire.begin();
  ioxt.begin();
  ioxt.write(MCP2301x::IODIRA, MCP2301x::ALL_OUTPUT);
  //ioxt.write(MCP2301x::GPPUA, MCP2301x::ALL_SET);
  ioxt.write(MCP2301x::IODIRB, MCP2301x::ALL_INPUT);
  ioxt.write(MCP2301x::GPPUB, MCP2301x::ALL_SET);

  Serial.println("Hello.");
  lastwatch = millis();
}

void loop() {
  byte a = millis()&0x07;
  
  ioxt.gpioA(1<<a);
}

