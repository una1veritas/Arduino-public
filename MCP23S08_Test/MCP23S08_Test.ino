#include <SPI.h>
#include <MCP23S08.h>

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_ShiftReg = 8,
};

enum LED7SEG {
  E   = 1<<0, 
  D   = 1<<1, 
  C   = 1<<2, 
  DP  = 1<<3, 
  B   = 1<<4,
  A   = 1<<5,
  F   = 1<<6,
  G   = 1<<7, 
};

const byte ten_secs[6] = {
  A, 
  B, 
  C, 
  D, 
  E, 
  F, 
};

MCP23S08 ioexpander(CS_MCP23S08, 0);

void output_test() {
  long sec = (millis()/1000);
  /*
  byte bits = ten_secs[((sec / 10) % 6)] | ((sec & 1) == 0 ? 0 : DP) | ten_secs[(sec / 300) % 6] | (((sec / 1800) & 1) == 0 ? 0 : G) ;
  */
  ioexpander.write( sec & 0xff );
  Serial.println(sec & 0xff);
}

void input_test() {
  uint8_t val = ioexpander.read(); //ioxt.read(ioxt.GPIO);
  Serial.println(val, HEX);
  delay(1000);
}

bool test_for_output = true;
void setup() {

  Serial.begin(115200);
  Serial.println("Hello.");

  digitalWrite(CS_23LC1024, HIGH);
  pinMode(CS_23LC1024, OUTPUT);
  digitalWrite(CS_ShiftReg, HIGH);
  pinMode(CS_ShiftReg, OUTPUT);
  digitalWrite(CS_MCP23S08, HIGH);
  pinMode(CS_MCP23S08, OUTPUT);

  SPI.begin();
  Serial.println("SPI started.");
  ioexpander.begin();
  Serial.println("IO Expander started.");

}

void loop() {
  if ( test_for_output ) { 
    ioexpander.set_gpio_output();
    Serial.println("Do output test.");
    //output_test();
    ioexpander.write(0x55);
    delay(500);
    ioexpander.write(0xaa);
    delay(500);
  } else {
    ioexpander.enable_gpio_pullup();
    ioexpander.set_gpio_input();
    Serial.println("Do input test.");
    // input_test();
    Serial.println(ioexpander.read(), HEX);
    delay(500);
    Serial.println(ioexpander.read(), HEX);
    delay(500);
  }
}
