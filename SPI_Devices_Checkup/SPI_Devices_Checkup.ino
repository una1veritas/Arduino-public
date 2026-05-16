#include <SPI.h>
#include <SPISRAM.h>
#include <MCP23S08.h>
#include <MCP23S17.h>

#include <ShiftRegister.h>

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
const int MCP23S08_CS = 9;
const int ShiftRegs_CS = 8;
// optional 
//const int SRAM_HOLD = 12; // / SIO3
//const int SRAM_SIO2 = 11;

SPISRAM SPIsram(SRAM_CS, SPISRAM::BUS_WIDTH_23LC1024); // CS pin
MCP23S08 io8expander(MCP23S08_CS, 0);

char buf128[128];

void setup() {
  
  Serial.begin(115200);
  while (!Serial) {}
  
  // All SPI devices must be inactivated
  // by pulling CSs high.

  // inactivate other SPI slaves
  digitalWrite(ShiftRegs_CS, HIGH);
  pinMode(ShiftRegs_CS, OUTPUT);
  digitalWrite(MCP23S08_CS, HIGH);
  pinMode(MCP23S08_CS, OUTPUT);
  digitalWrite(SRAM_CS, HIGH);
  pinMode(SRAM_CS, OUTPUT);
  // ensure the HOLD pulled up
  //pinMode(SRAM_HOLD, OUTPUT);
  //digitalWrite(SRAM_HOLD, HIGH);
  
  SPI.begin();
  SPIsram.begin();
  io8expander.begin();


  SPIsram_simple_test();
}

void loop()
{
  IOExpander_simple_test(millis()/500);
  delay(500);
}

void IOExpander_simple_test(uint8_t c) {
  io8expander.enable_gpio_pullup();
  io8expander.set_gpio_input();
  uint8_t inputval = io8expander,read();
  io8expander.set_gpio_output();
  io8expander.disable_gpio_pullup();
  io8expander.write(c);
  return inputval;
}
void SPIsram_simple_test() {
    Serial.println();
  Serial.println("Starting a test.");
  Serial.println();
  Serial.println("Byte write...");
  const char * str = "Hello, friends.";
  char * ptr;
  int ix;
  for(ix = 0, ptr = str; *ptr != 0; ++ix, ++ptr) {
    SPIsram.write(ix, *ptr);
  }

  Serial.println("Byte read...");
  for(ix = 0; ix < strlen(str); ++ix) {
    Serial.print((char) SPIsram[ix]);
  }
  Serial.println();

  Serial.println("\nblock read/write...");
  int addr = 0x800;
  char text[128] = "Awake, arise, or be forever fallen!";
  long textlen = strlen(text);
  Serial.println("block write...");
  SPIsram.write(addr, (byte*)text, textlen+1);

  memset((void*)text, '*', 128);
  Serial.println("block read...");
  SPIsram.read(addr, text, textlen+1);
  Serial.println( text );
}
void SPIsram_readwrite_test() {
  char text[128];
  randomSeed(analogRead(0));
  long count = 0, err = 0;
  Serial.println("\nRandom read/write...");
  while ( count < 1024 ) {
    uint32_t addr;
    addr = (uint32_t(random()) << 8 | random(0, 0xf)) & SPIsram.addressmask();
    
    snprintf(buf128, 127, "%06lX: ", addr);
    Serial.print(buf128);
    for(int i = 0; i < 16; i++) {
      text[i] = random(0xff);
      snprintf(buf128, 127, " %02x", uint8_t(text[i]) );
      Serial.print(buf128);
      SPIsram.write(addr+i, text[i]);
    }
    Serial.println( " / " );
    Serial.print( "        " );
    for(int i = 0; i < 16; i++) {
      snprintf(buf128, 127, " %02x", SPIsram.read(addr+i));
      Serial.print(buf128);
      count++;
      if ((byte)text[i] != SPIsram[addr+i]) err++;
    }
    Serial.println();
  }
  Serial.print("error count = ");
  Serial.print(err);
  Serial.print(" of ");
  Serial.println(count);
}