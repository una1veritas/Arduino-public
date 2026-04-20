#include <SPI.h>
#include <MCP23S08.h>
#include <MCP23S17.h>

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
};

enum {
  MEM_WE = 19,
  MEM_OE = 18,
  MEM_CE = 17,
};


MCP23S08 data_xt(CS_MCP23S08, 0);
MCP23S17 addr_xt(CS_MCP23S17, 1);

uint8_t mem_read(const uint16_t & addr) {
  uint8_t val;
  digitalWrite(MEM_WE, HIGH); // ensure
  addr_xt.gpio16_write(addr);
  digitalWrite(MEM_CE, LOW);
  delayMicroseconds(100);
  digitalWrite(MEM_OE, LOW);
  delayMicroseconds(50);
  val = data_xt.gpio_read();
  digitalWrite(MEM_OE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  return val;
}

uint8_t mem_write(const uint16_t & addr, uint8_t val) {
  digitalWrite(MEM_OE, HIGH); // ensure
  digitalWrite(MEM_WE, HIGH); // ensure
  addr_xt.gpio16_write(addr);
  digitalWrite(MEM_CE, LOW);
  data_xt.gpio_output();
  data_xt.disable_pullup();
  data_xt.gpio_write(val);
  delayMicroseconds(50);
  digitalWrite(MEM_WE, LOW);
  delayMicroseconds(50);
  digitalWrite(MEM_WE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  data_xt.enable_pullup();
  data_xt.gpio_input();
  return val;
}

const String data = 
  "Hickory dickory dock,\n" 
  "The mouse ran up the clock.\n"
  "The clock strucked one, \n"
  "The mouse ran down,\n"
  "Hickory dickory dock.\n";

void setup() {
  uint8_t val;
  char tmp[128];
  Serial.begin(9600);
  Serial.println("Hello.");

  // ensure to disable all the SPI slave devices. 
  pinMode(CS_23LC1024, OUTPUT); digitalWrite(CS_23LC1024, HIGH);
  pinMode(CS_MCP23S08, OUTPUT); digitalWrite(CS_MCP23S08, HIGH);
  pinMode(CS_MCP23S17, OUTPUT); digitalWrite(CS_MCP23S17, HIGH);

  pinMode(MEM_CE, OUTPUT); digitalWrite(MEM_CE, HIGH);
  pinMode(MEM_WE, OUTPUT); digitalWrite(MEM_WE, HIGH);
  pinMode(MEM_OE, OUTPUT); digitalWrite(MEM_OE, HIGH);

  SPI.begin();
  data_xt.begin();
  data_xt.enable_pullup();
  data_xt.gpio_input();
  addr_xt.begin();
  addr_xt.hw_address_enable();
  addr_xt.gpio16_output();
  Serial.println("SPI started. " "IO Expander started.");

  const bool write = true; 

  if (write) {
    Serial.println("\n\nWriting...");
    for (uint16_t addr = 0; addr < data.length(); ++addr) {
    val = mem_write(addr, data[addr]);
    if ( (addr & 0x0f) == 0) {
      snprintf(tmp, 127, "\n%02x: ", addr);
      Serial.print(tmp);
    }
    snprintf(tmp, 127, "%02x ", data[addr]);
    Serial.print(tmp);
    // if (val == 0)
    //   break;
    }
  }

  Serial.println("\n\nReading...");
  for (uint16_t addr = 0; addr < 0x80; ++addr) {
    val = mem_read(addr);
    if ( (addr & 0x0f) == 0) {
      snprintf(tmp, 127, "\n%02x: ", addr);
      Serial.print(tmp);
    }
    snprintf(tmp, 127, "%02x ", val);
    Serial.print(tmp);
    // if (val == 0)
    //   break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
