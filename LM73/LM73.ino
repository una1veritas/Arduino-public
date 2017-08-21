#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Hi.");
  Wire.begin();

  Wire.beginTransmission(0x98>>1);
  Wire.write(0x04);
  Wire.write(0x60);
  Wire.endTransmission();
  delay(500);  
  // put your setup code here, to run once:

}

void loop() {
  int16_t temp, signbit;
  // put your main code here, to run repeatedly: 
  Wire.beginTransmission(0x98>>1);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(0x98>>1, 2);
  temp = Wire.read();
  temp <<= 8;
  temp |= Wire.read();
  temp &= 0xfffc;
  /*
  signbit = temp & 0x8000;
  temp &= 0x7fff;
  temp >>= 2;
  temp |= signbit;
  */
  Serial.println((float)temp/128);
  delay(3000);  
}
