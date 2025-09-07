
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

struct SRAM {
  // pin connection
  const uint8 EN, _CS, _OE, _WE; // ~chip enable, ~output enable, ~write enable
  static const uint8 ABUS_WIDTH = 16;
  static const uint8 DBUS_WIDTH = 8;
  // address low & high -- PA, PC
  // data 8 bit --- PL
  static uint8 ABUS[ABUS_WIDTH] = {
    // PORTA
    22, 23, 24, 25, 26, 27, 28, 29,
    // PORTC
    37, 36, 35, 34, 33, 32, 31, 30,
    //2 
  };
  static uint8 DBUS[DBUS_WIDTH] = {
    // PORTL
    49, 48, 47, 46, 45, 44, 43, 42,
  };

  SRAM(uint8 en, uint8 _cs, uint8 _oe, uint8 _we)  : EN(en), _CS(_cs), _OE(_oe), _WE(_we) { 
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
    DATA_BUS_mode_input();
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

  void set_DATA_BUS(uint8 data) {
    /*
    for(uint8 bit = 0; bit < 8; ++bit) {
      digitalWrite(DBUS[bit], (data & 0x01) );
      data >>= 1;
    }
    */
    PORTL = data;
  }

  uint8 get_DATA_BUS() {
    uint8 val = 0;
    /*
    for (uint8 bit = DBUS_WIDTH; bit > 0; ) {
      val <<= 1;
      if (digitalRead(DBUS[--bit]) == HIGH) 
        val |= 0x01;
    }
    return val;
    */
    val = PINL;
    return val;
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

  void DATA_BUS_mode_input() {
    /*
    for(uint8 bit = 0; bit < DBUS_WIDTH; ++bit) {
      pinMode(DBUS[bit], INPUT);
      digitalWrite(DBUS[bit], LOW);
    }
    */
    DDRL = 0x00;
    PORTL = 0x00;
  }

  void DATA_BUS_mode_output() {
    /*
    for(uint8 bit = 0; bit < DBUS_WIDTH; ++bit) {
      pinMode(DBUS[bit], OUTPUT);
    }
    */
    DDRL = 0xff;
  }


  void write(uint32 addr, uint8 data) {
    // ensure the state
    output_disable();  // _OE = High
    write_disable();
    DATA_BUS_mode_output();
    //
    set_ADDR_BUS(addr);
    //delayMicroseconds(1); // Wait 1 u sec
    set_DATA_BUS(data);
    //delayMicroseconds(1); // Wait 1 u sec
    write_enable();   // _WE = Low
    __asm__ __volatile("nop"); // can be possibly omitted in write (1/16MHz = 62.5 ns)
    //delayMicroseconds(1); // Wait 1 u sec
    write_disable();  // _WE = High
  }

  uint8 read(uint32 addr) {
    uint8 val = 0;
    output_disable();
    write_disable();
    DATA_BUS_mode_input();
    //
    set_ADDR_BUS(addr);
    //delayMicroseconds(1); // Wait 1 u sec
    output_enable();    // _OE = Low, falling edge
    //delayMicroseconds(1); // Wait 1 u sec
    __asm__ __volatile("nop"); //(1/16MHz = 62.5 ns, seems works fine with 55ns chip)
    val = get_DATA_BUS();
    output_disable();
    return val;
  }
};

const uint8 MEGA_EN = 38; 
const uint8 MEGA_CS = 39; // PG2/ALE (_E1)
const uint8 MEGA_OE = 40; // PG1/RD. (_G)
const uint8 MEGA_WE = 41; // PG0/WR. (_W)
/*
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
*/
SRAM sram(MEGA_EN, MEGA_CS,MEGA_OE, MEGA_WE);
bool toggle_on = true;
uint32 test_addr = 0;
const uint32 MAX_ADDR = 0x1ffff;
unsigned long counter = 0;
const uint8 MEMBUFF_LEN = 128;
uint8 readbuff[MEMBUFF_LEN], writebuff[MEMBUFF_LEN];
char buf[32];

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

void buffer_dump(uint8 buffer[], const uint32 & bytes = 32, const uint32 baseaddr = 0x0000) {
  char buf[16];
  for(uint32 offset = 0; offset < bytes; ++offset) {
    if ( (offset & 0x0f) == 0 ) {
      snprintf(buf, 16, "%04X : ", baseaddr+offset);
      Serial.print(buf);
    }
    snprintf(buf, 16, "%02x ", buffer[offset]);
    Serial.print(buf);
    if ( (offset & 0x0f) == 0x0f ) {
      Serial.println();
    }
    
  }
}

uint8 crc8(const uint8 data[]) {
    uint8 crc = 0x00; // Initial value
    for (uint32 ix = 0; ix < MEMBUFF_LEN; ++ix) {
        crc ^= data[ix];
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07; // 0x07 is the CRC-8 polynomial
            else
                crc <<= 1;
        }
    }
    return crc;
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
  for(uint32 offset = 0; offset < MEMBUFF_LEN; ++offset) {
    readbuff[offset] = sram.read(test_addr + offset);
  }
  sram.deselect();
  buffer_dump(readbuff, MEMBUFF_LEN, test_addr);
  Serial.print("crc8 = ");
  Serial.println(crc8(readbuff), HEX);

  // write random bytes to memory
  sram.select();
  Serial.println(" Write: ");
  for(uint32 offset = 0; offset < MEMBUFF_LEN; ++offset) {
    writebuff[offset] = random(256);
    sram.write(test_addr + offset, writebuff[offset]);
  }
  sram.deselect();
  buffer_dump(writebuff, MEMBUFF_LEN, test_addr);
  Serial.print("crc8 = ");
  Serial.println(crc8(writebuff), HEX);
  // display memory
  sram.select();
  Serial.println("ReRead: ");
  for(uint32 offset = 0; offset < MEMBUFF_LEN; ++offset) {
    readbuff[offset] = 0;
    readbuff[offset] = sram.read(test_addr + offset);
  }
  sram.deselect();
  buffer_dump(readbuff, MEMBUFF_LEN, test_addr);
  Serial.print("crc8 = ");
  Serial.println(crc8(readbuff), HEX);


  // match?
  uint32 failure_count = 0;
  for(uint32 offset = 0; offset < MEMBUFF_LEN; ++offset) {
    if (readbuff[offset] != writebuff[offset]) {
      failure_count += 1;
      Serial.print("at ");
      Serial.print(test_addr + offset, HEX);
      Serial.print(" wriitten ");
      Serial.print(writebuff[offset], HEX);
      Serial.print(" and ");
      Serial.print(readbuff[offset], HEX);
      Serial.println(" differes.");
    }
  }
  if (failure_count > 0) {
    Serial.print("W/R Failure occurred ");
    Serial.print(failure_count);
    Serial.println(" times!!!\n");
    delay(3000);
    test_addr += MEMBUFF_LEN;
  } else {
    Serial.println("Ok.\n");
    test_addr += MEMBUFF_LEN + random(5)*MEMBUFF_LEN;
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
