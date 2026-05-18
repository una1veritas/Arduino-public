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
  digitalWrite(8, HIGH);
  pinMode(8, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(9, OUTPUT);
  
  SPI.begin();
  myRAM.begin();
}

void loop()
{
}

