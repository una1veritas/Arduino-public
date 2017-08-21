#include <SPI.h>
#include <SPISRAM.h>

const int SRAM_CS_PC6 = 5; // PC6

SPISRAM sram(SRAM_CS_PC6, SPISRAM::BUS_MBits);

void setup() {
  // put your setup code here, to run once:

  pinMode(20, OUTPUT);
  digitalWrite(20, HIGH);
  
  while( !Serial) {}

  SPI.begin();
  sram.begin();

  Serial.println("Started.");
  sram.write(0x1f3, '#');
  Serial.println((char)sram.read(0x1f3));
  char str[] = "This is a pen.";
  char buf[128];
  int len = strlen(str);
  sram.write(0x01f, str, len);
  sram.read(0x01f, buf, len);
  buf[len]=0;
  Serial.println(buf);
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

}
