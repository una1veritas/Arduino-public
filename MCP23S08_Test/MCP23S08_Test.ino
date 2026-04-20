#include <SPI.h>
#include <MCP23S08.h>

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
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

MCP23S08 ioxt(CS_MCP23S08, 0);

void output_test() {
  long sec = (millis()/1000);
  byte bits = ten_secs[((sec / 10) % 6)] | ((sec & 1) == 0 ? 0 : DP) | ten_secs[(sec / 300) % 6] | (((sec / 1800) & 1) == 0 ? 0 : G) ;
  ioxt.write_gpio( bits ^ 0xff );
}
void input_test() {
  uint8_t val = ioxt.read_gpio(); //ioxt.read(ioxt.GPIO);
  Serial.println(val, HEX);
  delay(1000);
}

void setup() {
  bool test_for_output = false;

  Serial.begin(9600);
  Serial.println("Hello.");

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  SPI.begin();
  Serial.println("SPI started.");
  ioxt.begin();
  Serial.println("IO Expander started.");
  
  if (test_for_output) { 
    ioxt.gpio_output();
    for(;;) {
      output_test();
    }
  } else if (!test_for_output) {
    ioxt.enable_pullup();
    ioxt.gpio_input();
    for(;;) {
      input_test();
    }
  }
}

void loop() {

}
