
typedef uint8_t uint8;
typedef uint16_t uint16;

struct SRAM {
  // pin connection
  const uint8 _WE = 41; // PG0/WR. (_W)
  const uint8 _OE = 40; // PG1/RD. (_G)
  const uint8 _CS = 39; // PG2/ALE (_E1)
  static const uint8 ADDR_BUS_SIZE = 17;
  const uint8 ADDR_BUS[ADDR_BUS_SIZE] = {
    // PORTA
    22, 23, 24, 25, 26, 27, 28, 29,
    // PORTC
    37, 36, 35, 34, 33, 32, 31, 30,
    // PORTD
    38 
  };
  static const uint8 DATA_BUS_SIZE = 8;
  const uint8 DATA_BUS[DATA_BUS_SIZE] = {
    // PORTL
    49, 48, 47, 46, 45, 44, 43, 42,
  };

  void init() {
    // set pin mode
    pinMode(_CS, OUTPUT);
    disable();
    pinMode(_OE, OUTPUT);
    output_disable();
    pinMode(_WE, OUTPUT);
    write_disable();
    for (uint8 bit = 0; bit < ADDR_BUS_SIZE; ++bit) {
      pinMode(ADDR_BUS[bit], OUTPUT); // address bus
    }
    DATA_BUS_mode(INPUT);
  }

  void set_ADDR_BUS(uint16 addr) {
    for(uint8 bit = 0; bit < ADDR_BUS_SIZE; ++bit) {
      if (addr & 0x0001) {
        digitalWrite(ADDR_BUS[bit], HIGH);
      } else {
        digitalWrite(ADDR_BUS[bit], LOW);
      }
      addr >>= 1;
    }
  }

  void enable(void) {
    digitalWrite(_CS, LOW);
  }

  void disable(void) {
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
      for(uint8 bit = 0; bit < DATA_BUS_SIZE; ++bit) {
        pinMode(DATA_BUS[bit], OUTPUT);
      }
    } else {
      for(uint8 bit = 0; bit < DATA_BUS_SIZE; ++bit) {
        pinMode(DATA_BUS[bit], INPUT);
        digitalWrite(DATA_BUS[bit], LOW);
      }
    }
  }

  void write(uint16 addr, uint8 data) {
    output_disable();  // _OE = High
    write_disable();
    DATA_BUS_mode(OUTPUT);
    set_ADDR_BUS(addr);
    for(uint8 bit = 0; bit < 8; ++bit) {
      if (data & 0x01) {
        digitalWrite(DATA_BUS[bit], HIGH);
      } else {
        digitalWrite(DATA_BUS[bit], LOW);
      }
      data >>= 1;
    }
    write_enable();   // _WE = Low
    delayMicroseconds(1); // Wait 1 u sec
    write_disable();  // _WE = High
  }

  uint8 read(uint16 addr) {
    uint8 val = 0;
    uint8 bit = 0x01;
    DATA_BUS_mode(INPUT);
    output_disable();
    write_disable();
    set_ADDR_BUS(addr);
    output_enable();    // _OE = Low, falling edge
    delayMicroseconds(1); // Wait 1 u sec
    for (uint8 bpos = 0; bpos < DATA_BUS_SIZE; ++bpos ) {
      if (digitalRead(DATA_BUS[bpos]) == HIGH) val |= bit;
      bit <<= 1;
    }
    output_disable();
    return val;
  }
};

SRAM sram;
bool toggle_on = true;
unsigned long test_addr = 0;
unsigned long counter = 0;

void setup() {
  // put your setup code here, to run once:
  sram.init();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(38400);
  while (! Serial) {}
  randomSeed(1);
  Serial.println("SRAM TEST PROGRAM");
  delay(1000);
  Serial.println("START!!");
}

void loop() {
  test_addr += random(0x1FFFF);
  test_addr &= 0x1FFFF;
  // display address
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(test_addr, HEX);
  Serial.print(" Read: ");

  // display memory
  sram.enable();
  uint8 rdata = sram.read(test_addr);
  Serial.print(rdata, HEX);

  // random write memory
  Serial.print(" Write: ");
  uint8 wdata = random(256);
  Serial.print(wdata, HEX);
  sram.write(test_addr, wdata);
  /*
  Serial.print(" PORTL ");
  Serial.print(PORTL, HEX);
  Serial.print(" PORTA ");
  Serial.print(PORTA, HEX);
  Serial.print(" PORTC ");
  Serial.print(PORTC, HEX);
  Serial.print(" PORTG ");
  Serial.print(PORTG, HEX);
 */
  // display memory
  Serial.print(" Read: ");
  rdata = 0x00;
  rdata = sram.read(test_addr);
  sram.disable();
  Serial.print(rdata, HEX);
  // match?
  if (wdata == rdata) {
    Serial.println("  OK");
  } else {
    Serial.println("  ERROR!!");
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
