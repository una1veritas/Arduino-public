
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

struct SRAM {
  // pin connection
  const uint8 EN, _CS, _OE, _WE; // ~chip enable, ~output enable, ~write enable
  const uint8 ABUS_WIDTH = 17;
  const uint8 * ABUS;
  const uint8 DBUS_WIDTH = 8;
  const uint8 * DBUS;

  SRAM(uint8 en, uint8 _cs, uint8 _oe, uint8 _we, 
    uint8 addr_bus_width, uint8 addr_bus[], 
    uint8 data_bus_width, uint8 data_bus[]) 
    : EN(en), _CS(_cs), _OE(_oe), _WE(_we), 
    ABUS_WIDTH(addr_bus_width), ABUS(addr_bus), DBUS_WIDTH(data_bus_width), DBUS(data_bus) { 
      init();
    }

  void init() {
    // set pin mode
    pinMode(EN, OUTPUT);
    enable();
    pinMode(_CS, OUTPUT);
    deselect();
    pinMode(_OE, OUTPUT);
    output_disable();
    pinMode(_WE, OUTPUT);
    write_disable();
    for (uint8 bit = 0; bit < ABUS_WIDTH; ++bit) {
      pinMode(ABUS[bit], OUTPUT); // address bus
    }
    DATA_BUS_mode(INPUT);
  }

  void set_ADDR_BUS(uint32 addr) {
    for(uint8 bit = 0; bit < ABUS_WIDTH; ++bit) {
      if (addr & 0x0001) {
        digitalWrite(ABUS[bit], HIGH);
      } else {
        digitalWrite(ABUS[bit], LOW);
      }
      addr >>= 1;
    }
  }

  void enable(void) {
    digitalWrite(EN, HIGH);
  }

/*  void disable(void) {
    digitalWrite(EN, LOW);
  }
*/
  void select(void) {
    digitalWrite(_CS, LOW);
  }

  void deselect(void) {
    digitalWrite(_CS, HIGH);
  }

  void output_enable() {
    digitalWrite(_OE, LOW);
  }

  void output_disable() {
    digitalWrite(_OE, HIGH);
  }

  void write_enable() {
    digitalWrite(_WE, LOW);
  }

  void write_disable() {
    digitalWrite(_WE, HIGH);
  }

  void DATA_BUS_mode(uint8 in_out) {
    if (in_out == OUTPUT) {
      for(uint8 bit = 0; bit < DBUS_WIDTH; ++bit) {
        pinMode(DBUS[bit], OUTPUT);
      }
    } else {
      for(uint8 bit = 0; bit < DBUS_WIDTH; ++bit) {
        pinMode(DBUS[bit], INPUT);
        digitalWrite(DBUS[bit], LOW);
      }
    }
  }

  void write(uint32 addr, uint8 data) {
    // ensure the state
    output_disable();  // _OE = High
    write_disable();
    //
    DATA_BUS_mode(OUTPUT);
    set_ADDR_BUS(addr);
    for(uint8 bit = 0; bit < 8; ++bit) {
      digitalWrite(DBUS[bit], (data & 0x01) );
      data >>= 1;
    }
    write_enable();   // _WE = Low
    __asm__ __volatile("nop"); // can be possibly omitted in write (1/16MHz = 62.5 ns)
    //delayMicroseconds(1); // Wait 1 u sec
    write_disable();  // _WE = High
  }

  uint8 read(uint32 addr) {
    uint8 val = 0;
    DATA_BUS_mode(INPUT);
    output_disable();
    write_disable();
    set_ADDR_BUS(addr);
    output_enable();    // _OE = Low, falling edge
    //delayMicroseconds(1); // Wait 1 u sec
    __asm__ __volatile("nop"); //(1/16MHz = 62.5 ns, seems works fine with 55ns chip)
    for (uint8 bit = DBUS_WIDTH; bit > 0; ) {
      val <<= 1;
      if (digitalRead(DBUS[--bit]) == HIGH) 
        val |= 0x01;
    }
    output_disable();
    return val;
  }
};

const uint8 MEGA_EN = 38; 
const uint8 MEGA_CS = 39; // PG2/ALE (_E1)
const uint8 MEGA_OE = 40; // PG1/RD. (_G)
const uint8 MEGA_WE = 41; // PG0/WR. (_W)
const uint8 ADDR_BUS_WIDTH = 17;
const uint8 MEGA_ADDR_BUS[ADDR_BUS_WIDTH] = {
  // PORTA
  22, 23, 24, 25, 26, 27, 28, 29,
  // PORTC
  37, 36, 35, 34, 33, 32, 31, 30,
  
  2 
};
const uint8 DATA_BUS_WIDTH = 8;
const uint8 MEGA_DATA_BUS[DATA_BUS_WIDTH] = {
  // PORTL
  49, 48, 47, 46, 45, 44, 43, 42,
};

SRAM sram(MEGA_EN, MEGA_CS,MEGA_OE, MEGA_WE, ADDR_BUS_WIDTH, MEGA_ADDR_BUS, DATA_BUS_WIDTH, MEGA_DATA_BUS);
bool toggle_on = true;
uint32 test_addr = 0;
const uint32 MAX_ADDR = 0x1ffff;
unsigned long counter = 0;
uint8 readbuff[32], writebuff[32];
const uint8 MEMBUFF_LEN = 32;
char buf[16];

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(MEGA_EN, OUTPUT);
  digitalWrite(MEGA_EN, HIGH);
  Serial.begin(38400);
  while (! Serial) {}
  randomSeed(1);
  Serial.println("SRAM TEST PROGRAM");
  delay(1000);
  Serial.println("START!!");
}

void Serial_out(uint8 bytes[], uint8 num) {
  // display memory
  for(uint32 i = 0; i < num; ++i) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%02X ", bytes[i]);
    Serial.print(buf);
  }
  Serial.println();
}
void loop() {
  test_addr &= MAX_ADDR;
  // display address
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Address ");
  Serial.print(test_addr, HEX);
  Serial.println(" read out: ");

  // display memory
  sram.select();
  for(uint32 offset = 0; offset < 32; ++offset) {
    readbuff[offset] = sram.read(test_addr + offset);
  }
  Serial_out(readbuff, 32);

  // random write memory
  Serial.println(" Write: ");
  for(uint32 offset = 0; offset < 32; ++offset) {
    writebuff[offset] = random(256);
    sram.write(test_addr + offset, writebuff[offset]);
  }
  Serial_out(writebuff, 32);
  // display memory
  Serial.println("ReRead: ");
  for(uint32 offset = 0; offset < 32; ++offset) {
    readbuff[offset] = 0;
    readbuff[offset] = sram.read(test_addr + offset);
  }
  Serial_out(readbuff, 32);
  sram.deselect();

  // match?
  uint32 failure_count = 0;
  for(uint32 offset = 0; offset < 32; ++offset) {
    if (readbuff[offset] != writebuff[offset]) {
      failure_count += 1;
    }
  }
  if (failure_count > 0) {
    Serial.print("W/R Failure occurred ");
    Serial.print(failure_count);
    Serial.println(" times!!!\n");
  } else {
    Serial.println("Ok.\n");
    test_addr += 0x0130;
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
