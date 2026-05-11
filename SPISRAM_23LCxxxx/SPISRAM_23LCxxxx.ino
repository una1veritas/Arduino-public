#include <SPI.h>
#include <SPISRAM.h>

/*
 SRAM   Arduino
 1 CS   10(CS)  // must be initialized by programmer
 2 SO   12(MISO)
 3 -    -
 4 Vss  GND
 5 SI   11(MOSI)
 6 SCK  13(SCK)
 7 HOLD <-- pull up
 8 Vcc  
 */
const int SRAM_CS = 10;
// optional 
//const int SRAM_HOLD = 12; // / SIO3
//const int SRAM_SIO2 = 11;

SPISRAM myRAM(SRAM_CS, SPISRAM::BUS_WIDTH_23LC1024); // CS pin

char buf128[128];

void setup() {
  
  Serial.begin(115200);
  while (!Serial) {}
  
  // All SPI devices must be inactivated
  // by pulling CSs high.

  // inactivate other SPI slaves
  digitalWrite(A3, HIGH);
  pinMode(A3, OUTPUT);
  
  // set pinmode for CS
  digitalWrite(SRAM_CS, HIGH);
  pinMode(SRAM_CS, OUTPUT);
  // ensure the HOLD pulled up
  //pinMode(SRAM_HOLD, OUTPUT);
  //digitalWrite(SRAM_HOLD, HIGH);
  
  SPI.begin();
  myRAM.begin();

  Serial.println();
  Serial.println();
  Serial.println("Starting a test.");
  Serial.println("Byte write...");

  const char * str = "Hello, friends.";
  char * ptr;
  int ix;
  for(ix = 0, ptr = str; *ptr != 0; ++ix, ++ptr) {
    myRAM.write(ix, *ptr);
  }

  Serial.println("Byte read...");
  for(ix = 0; ix < strlen(str); ++ix) {
    Serial.print((char) myRAM[ix]);
  }
  Serial.println();

  Serial.println("\nByte write...");
  myRAM.write(0x7FFC,'W');
  myRAM.write(0x7FFD,'o');
  myRAM.write(0x7FFE,'r');
  myRAM.write(0x7FFF,'l');
  myRAM.write(0x8000,'d');
  myRAM.write(0x8001,'!');
  myRAM.write(0x8002,'!');

  Serial.println("Byte read...");
  Serial.print((char)myRAM[0x7FFC]);
  Serial.print((char)myRAM[0x7FFD]);
  Serial.print((char)myRAM.read(0x7FFE));
  Serial.print((char)myRAM.read(0x7FFF));
  Serial.print((char)myRAM.read(0x8000));
  Serial.print((char)myRAM.read(0x8001));
  Serial.print((char)myRAM.read(0x8002));

  Serial.println("\nblock write...");
  int addr = 0x7F00;
  char text[128] = "Awake, arise, or be forever fallen!";
  long textlen = strlen(text);
  myRAM.write(addr, (byte*)text, textlen+1);

  memset((void*)text, '*', 128);
  Serial.println("block read...");
  myRAM.read(addr, text, textlen+1);
  Serial.println( text );

randomSeed(analogRead(0));
  long count = 0, err = 0;
  Serial.println("\nRandom read/write...");
  while ( count < 1024 ) {
    uint32_t addr;
    addr = (uint32_t(random()) << 8 | random(0, 0xf)) & myRAM.addressmask();
    
    snprintf(buf128, 127, "%06lX: ", addr);
    Serial.print(buf128);
    for(int i = 0; i < 16; i++) {
      text[i] = random(0xff);
      snprintf(buf128, 127, " %02x", uint8_t(text[i]) );
      Serial.print(buf128);
      myRAM.write(addr+i, text[i]);
    }
    Serial.println( " / " );
    Serial.print( "        " );
    for(int i = 0; i < 16; i++) {
      snprintf(buf128, 127, " %02x", myRAM.read(addr+i));
      Serial.print(buf128);
      count++;
      if ((byte)text[i] != myRAM[addr+i]) err++;
    }
    Serial.println();
  }
  Serial.print("error count = ");
  Serial.print(err);
  Serial.print(" of ");
  Serial.println(count);
}

void loop()
{
}

