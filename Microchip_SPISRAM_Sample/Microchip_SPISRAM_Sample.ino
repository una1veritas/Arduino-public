#include <SPI.h>
#include <SPISRAM.h>

/*
 SRAM   Arduino
 1 CS   10(CS)
 2 SO   12(MISO)
 3 -    -
 4 Vss  GND
 5 SI   11(MOSI)
 6 SCK  13(SCK)
 7 HOLD <-- 100k ohm -- 3.3V
 8 Vcc  3.3V
 */
const int SRAM_CS = 53;

SPISRAM myRAM(SRAM_CS, SPISRAM::BUS_MBits); // CS pin
char buffer[128];

void setup() {
  char tmp[16];
  
  Serial.begin(57600);
  
  SPI.begin();
  myRAM.begin();
  
  Serial.println("Byte write...");
  myRAM.write(0, (byte*)"Hello, boy!", 12);  
  Serial.println("Byte read...");
  myRAM.read(0,(byte*)tmp, 12);
  Serial.println(tmp);

  Serial.println("\nByte write...");
  myRAM.write(0x7FFC,'W');
  myRAM.write(0x7FFD,'o');
  myRAM.write(0x7FFE,'r');
  myRAM.write(0x7FFF,'l');
  myRAM.write(0x8000,'d');
  myRAM.write(0x8001,'!');
  myRAM.write(0x8002,'!');

  Serial.println("Byte read...");
  Serial.println((char)myRAM.read(0x7FFC));
  Serial.println((char)myRAM.read(0x7FFD));
  Serial.println((char)myRAM.read(0x7FFE));
  Serial.println((char)myRAM.read(0x7FFF));
  Serial.println((char)myRAM.read(0x8000));
  Serial.println((char)myRAM.read(0x8001));
  Serial.println((char)myRAM.read(0x8002));

  Serial.println("\nseq write...");
  int addr = 0x7F00;
  myRAM.write(addr, (byte*)"Hello world!!", sizeof("Hello world!!"));

  Serial.println("seq read...");
  myRAM.read(addr, (byte*)buffer, sizeof(buffer));
  Serial.println( buffer );

}

void loop()
{
  long err = 0;
  uint8_t val0, val1;
  Serial.println("\nRandom read/write...");
  for(int i=0; i < 100; i++){
    int addr = random() & 0x7fff;
    val0 = addr^i;
    val1 = ~val0;
    Serial.print( addr, HEX );
    Serial.print( " " );
    Serial.print( val0, HEX );
    Serial.print( " " );
    Serial.print( val1, HEX );
    myRAM.write(addr, val0);
    myRAM.write(addr+1, val1);
    Serial.print( " " );
    Serial.print( myRAM.read(addr), HEX );
    Serial.print( " " );
    Serial.print( myRAM.read(addr+1), HEX );
    if (val0 != myRAM.read(addr) || val1 != myRAM.read(addr+1) ) { 
      err++;
      Serial.print(" !");
    }
    Serial.println();
  }
  Serial.print("error count = ");
  Serial.println(err);
  delay(1000);
}

