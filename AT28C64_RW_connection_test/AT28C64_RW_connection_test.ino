#include <SPI.h>
#include <MCP23S08.h>
#include <MCP23S17.h>

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
};

enum {
  MEM_WE = 16,
  MEM_OE = 15,
  MEM_CE = 14,
};

enum {
  AT28C64_IODIR_BITS = 0xe000,
  AT28C64_GPIO_HIGH_MASK = 0xe000, // or-ed with value
};

MCP23S08 databusx(CS_MCP23S08, 0);
MCP23S17 addrbusx(CS_MCP23S17, 1);

uint8_t mem_read(const uint16_t & addr) {
  uint8_t val;
  digitalWrite(MEM_WE, HIGH); // ensure
  addrbusx.write_GPIO16(addr);
  digitalWrite(MEM_CE, LOW);
  digitalWrite(MEM_OE, LOW);
  val = databusx.read_GPIO();
  digitalWrite(MEM_OE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  return val;
}

uint8_t AT28Cxx_byte_write(const uint16_t & addr, const uint8_t val) {
  digitalWrite(MEM_OE, HIGH); // ensure
  addrbusx.write_GPIO16(addr);
  databusx.write_IODIR(databusx.IODIR_OUTPUT8);
  databusx.disable_pullup();
  digitalWrite(MEM_CE, LOW);
  digitalWrite(MEM_WE, LOW);
  __asm__ __volatile__ ("nop\n\t");   // 62.5ns for t_AH min = 50ns
  databusx.write_GPIO(val);
  __asm__ __volatile__ ("nop\n\t");   // 62.5ns for t_DS min = 50ns
  // t_WP > 100ns
  digitalWrite(MEM_WE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  databusx.enable_pullup();
  databusx.write_IODIR(databusx.IODIR_INPUT8);

  // DATA polling to observe the end of this write cycle
  uint8_t t;
  //uint8_t count = 0;
  digitalWrite(MEM_CE, LOW);
  digitalWrite(MEM_OE, LOW);
  do {
    t = databusx.read_GPIO();
  } while ( t != val );
  digitalWrite(MEM_OE, HIGH);
  digitalWrite(MEM_CE, HIGH);
  //Serial.println(count);
  return t;
}

/* page write is available on AT28CxxB-15XU, not available on AT28C64 15PC
uint8_t at28cxx_page_write(const uint16_t & start_addr, const uint8_t vals[64], const uint8_t nbytes) {
  uint16_t base_addr = start_addr & 0xffc0;
  uint16_t bytecount = start_addr & 0x3f;

  digitalWrite(MEM_OE, HIGH); // ensure
  for( ; bytecount < min(nbytes, 64); ++bytecount) {
    uint8_t val = vals[bytecount];
    addrbusx.write_GPIO16(base_addr + bytecount);
    databusx.GPIO_output();
    databusx.disable_pullup();
    digitalWrite(MEM_CE, LOW);
    digitalWrite(MEM_WE, LOW);
    __asm__ __volatile__ ("nop\n\t");   // 62.5ns for t_AH min = 50ns
    databusx.write_GPIO(val);
    __asm__ __volatile__ ("nop\n\t");   // 62.5ns for t_DS min = 50ns
    // t_WP > 100ns
    digitalWrite(MEM_WE, HIGH);
    digitalWrite(MEM_CE, HIGH);

    // DATA polling to observe the end of this write cycle
    uint8_t t;
    digitalWrite(MEM_CE, LOW);
    databusx.enable_pullup();
    databusx.GPIO_input();
    digitalWrite(MEM_OE, LOW);
    do {
      t = databusx.read_GPIO();
    } while ( t == (val ^ 0x80) );
    digitalWrite(MEM_OE, HIGH);
    digitalWrite(MEM_CE, HIGH);
  }
  databusx.enable_pullup();
  databusx.GPIO_input();

  return bytecount;
}
*/

const String textdata = 
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
  databusx.begin();
  // data bus ioxt default setting 
  databusx.enable_pullup();
  databusx.write_IODIR(databusx.IODIR_INPUT8);

  addrbusx.begin();
  // addr bus ioxt default setting 
  //addrbusx.hw_address_enable();
  addrbusx.write_IODIR16(AT28C64_IODIR_BITS); // 1 input/0 output, 
  // A0 -- A12 is active, A13 is NC, A14 (pin 1) is NC or RDY/BUSY
  addrbusx.write_GPPU16(AT28C64_IODIR_BITS); // 1 input/0 output, 

  Serial.println("SPI and IO Extender started.");

  const bool write = true; 

  Serial.println("\n\nTesting...");
  uint16_t baseaddr = 0x0000;
  uint8_t data;

  if (write) {
    // writing test
    Serial.println("\nwriting");
    
    for(uint16_t i = 0; i < textdata.length(); ++i) {    
      // if ((i & 0x0f) == 0 ) {
      //   snprintf(tmp, 127, "%04x: ", baseaddr + i);
      //   Serial.print(tmp);
      // }
      uint8_t val = AT28Cxx_byte_write(baseaddr + i, textdata.c_str()[i]);
      snprintf(tmp, 127, "%02x/%02x ", textdata.c_str()[i], val);
      Serial.print(tmp);
      // snprintf(tmp, 127, "%02x %c ", textdata[i],  (isprint(textdata[i]) ? textdata[i] : ' ') );
      // Serial.print(tmp);
      if ((i & 0x0f) == 0x0f ) {
        Serial.println();
      }
    }
    
    //at28cxx_page_write(baseaddr, textdata.c_str(), min(textdata.length() + 1, 64));
  }

  // reading test
  Serial.println("\nreading");
  for(uint16_t i = 0; i < 0x100; ++i) {    
    if ((i & 0x0f) == 0 ) {
      snprintf(tmp, 127, "%04x: ", baseaddr + i);
      Serial.print(tmp);
    }
    uint8_t val = mem_read(baseaddr + i);
    snprintf(tmp, 127, "%02x %c ", val, (i < textdata.length() ? (val == textdata[i] ? 'o' : 'X') : ' ') );
    Serial.print(tmp);
    if ((i & 0x0f) == 0x0f ) {
      Serial.println();
    }
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
