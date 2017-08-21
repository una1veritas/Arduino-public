#include "gpio.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("base = "); Serial.println(PD13, HEX);
  Serial.print("PORT = "); Serial.println((uint32_t)PD13>>8&0xff, HEX);

  pinmode(PD11|PD13, OUTPUT);
  pinlow(PD13);
  pinhigh(PD11);
}

void loop() {
  // put your main code here, to run repeatedly: 
  pintoggle(PD11|PD13);
  delay(500);
}

