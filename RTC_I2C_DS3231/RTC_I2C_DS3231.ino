#include <Wire.h>


void read(byte addr, byte * data, byte num) {
   Wire.beginTransmission(0x68);
   Wire.write(addr);
  Wire.endTransmission();

  // request num bytes of data
  Wire.requestFrom((uint8_t)0x68, num);
  for(int i = 0; i < num; i++) {
    // store data in raw bcd format
    *data++ = Wire.read(); //Wire.receive();
  }
}

void write(byte addr, byte *data, byte num){
  Wire.beginTransmission(0x68);
  Wire.write(addr); // reset register pointer
  for(int i=0; i<num; i++) {
    Wire.write(*data++);
  }
  Wire.endTransmission();
}

  byte data[8];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);

  Wire.begin();
  Serial.println("Read Control Register, Status Register");
  read(0x0e,data, 2);
  Serial.println(data[0], HEX);
  Serial.println(data[1], HEX);
  byte cr = data[1];
  if ( data[1] & 0x80 ) {
    Serial.println("Oscillator has been stopped.");
    data[0] = 0x00;
    data[1] = 0x07;
    data[2] = 0x15;
    data[3] = 0x07;
    data[4] = 0x20;
    data[5] = 0x08;
    data[6] = 0x17;
     
    write(0x00,data,7);
    cr &= ~0x80;
    data[0] = cr;
    write(0x0f,data,1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  read(0x00,data,3);
  for(int i = 0; i < 3; i++) {
    Serial.print(data[2-i],HEX);
    if ( i != 2) Serial.print(":");
  }
  Serial.println();
  delay(1000);
}
