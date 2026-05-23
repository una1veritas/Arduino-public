#include <SPI.h>
#include <ShiftRegister.h>

ShiftRegister sreg(8, 7, ShiftRegister::MSB_FIRST);

uint32_t val = random(0x1000000);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  while( ! Serial);

  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  
  Serial.println("595 ShiftRegister Output Expander test.");
  SPI.begin();
  sreg.begin();
  sreg.output_enable();
  long swatch = micros();
  for(long i = 0; i < 1000; ++i) {
    sreg.write_bytes((uint8_t *) &i, 3);
  }
  Serial.println("Measure 1000 times loop: ");
  swatch = micros() - swatch;
  Serial.print("loops write(bytes( , 3) 1000 times took ");
  Serial.print(swatch);
  Serial.println(" micro sec.");
}

void loop() {
  // put your main code here, to run repeatedly:
  val = micros()>>8;
  sreg.write_bytes((uint8_t *) &val, 3);
  // if ( (millis()>>9) % 4 == 0 )
  //   sreg.output_enable();
  // else {
  //   sreg.output_disable();
  // }
}
