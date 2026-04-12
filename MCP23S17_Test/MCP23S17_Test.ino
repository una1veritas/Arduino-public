#include <SPI.h>
#include <MCP23S17.h>

MCP23S17 ioxt(9);


void setup() {
  Serial.begin(9600);
  Serial.println("Hello.");

  pinMode(10, OUTPUT);
  SPI.begin();
  Serial.println("SPI started.");
//  ioxt.begin();
  Serial.println("IO Expander started.");
  
  //ioxt.mode(ioxt.GPIOA, ioxt.ALL_OUTPUT);
}

void loop() {
  long sec = millis();
/*  ioxt.gpioA(millis()>>4);
  delayMicroseconds(100);
*/}


