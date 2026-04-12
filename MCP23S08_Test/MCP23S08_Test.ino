#include <SPI.h>
#include <MCP23S08.h>

MCP23S08 ioxt(9, 0);


void setup() {
  Serial.begin(9600);
  Serial.println("Hello.");

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);

  SPI.begin();
  Serial.println("SPI started.");
  ioxt.begin();
  Serial.println("IO Expander started.");
  
  ioxt.set_output();
}

void loop() {
  long sec = millis()/1000;
  ioxt.write_gpio(sec & 0xff);
}


