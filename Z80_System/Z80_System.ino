typedef uint8_t uint8;
typedef uint16_t uint16;

class MPU_Z80 {
  static const uint8 IOADDR_BUS_WIDTH = 8;
  static const uint8 ADDR_BUS_WIDTH = 16;
  static const uint8 DATA_BUS_WIDTH = 8;
  //
  const uint8 CLK, _INT, _NMI, _WAIT, _BUSREQ, _RESET; // input
  const uint8 _HALT, _MREQ, _IOREQ, _RD, _WR, _BUSACK, _M1, _RFSH; //__output
  const uint8 * ABUS;
  const uint8 * DBUS;

public:
  MPU_Z80(const uint8 clk, uint8 _int, uint8 _nmi, uint8 _wait, uint8 busreq, uint8 _rst, 
    uint8 _mreq, uint8 _rd, uint8 _wr, uint8 _m1, uint8 _rfsh,
    uint8 addr_bus[], uint8 data_bus[]) 
  : CLK(clk), _INT(_int), _NMI(_nmi), _WAIT(_wait), _BUSREQ(busreq), _RESET(_rst),
  _MREQ(_mreq), _RD(_rd), _WR(_wr), _M1(_m1), _RFSH(_rfsh), 
  ABUS(addr_bus), DBUS(data_bus) {
    init();
  }

  void init() {
    pinMode(CLK, OUTPUT);
    pinMode(_BUSREQ, OUTPUT);
    pinMode(_INT, OUTPUT);
    pinMode(_NMI, OUTPUT);
    pinMode(_WAIT, OUTPUT);
    pinMode(_BUSREQ, OUTPUT);
    pinMode(_RESET, OUTPUT);

    digitalWrite(_BUSREQ, HIGH);
    digitalWrite(_INT, HIGH);
    digitalWrite(_NMI, HIGH);
    digitalWrite(_WAIT, HIGH);
    digitalWrite(_BUSREQ, HIGH);
    digitalWrite(_RESET, HIGH);

    pinMode(_MREQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_M1, INPUT);
    pinMode(_RFSH, INPUT);

    for(uint8 pix = 0; pix < ADDR_BUS_WIDTH; ++pix) {
      pinMode(ABUS[pix], INPUT);
      digitalWrite(ABUS[pix], HIGH);
    }
    DATA_BUS_mode(INPUT);
  }

  void reset() {
    digitalWrite(_RESET, LOW);
    for(uint8 i = 0; i < 8; ++i) {
      toggle_clock();
      delay(1);
    }
    digitalWrite(_RESET, HIGH);
  }

  void toggle_clock() {
    if (digitalRead(CLK) == HIGH)
      digitalWrite(CLK, LOW);
    else
      digitalWrite(CLK, HIGH);
  }

  uint8 clock() {
    return digitalRead(CLK);
  }

  uint8 MREQ() {
    return digitalRead(_MREQ);
  }

  uint8 RD() {
    return digitalRead(_RD);
  }

  uint8 WR() {
    return digitalRead(_WR);
  }

  uint8 M1() {
    return digitalRead(_M1);
  }

  uint16 get_ADDR_BUS() {
    uint16 val = 0;
    for (uint8 bit = ADDR_BUS_WIDTH; bit > 0; ) {
      val <<= 1;
      if (digitalRead(ABUS[--bit]) == HIGH) 
        val |= 0x01;
    }
    return val;
  }

  void DATA_BUS_mode(uint8 in_out) {
    if (in_out == OUTPUT) {
      for(uint8 bit = 0; bit < DATA_BUS_WIDTH; ++bit) {
        pinMode(DBUS[bit], OUTPUT);
      }
    } else {
      for(uint8 bit = 0; bit < DATA_BUS_WIDTH; ++bit) {
        pinMode(DBUS[bit], INPUT);
        //digitalWrite(DBUS[bit], HIGH);
      }
    }
  }

  void set_DATA_BUS(uint8 data) {
    for (uint8 bit = 0; bit < DATA_BUS_WIDTH; ++bit) {
      digitalWrite(DBUS[bit], (data & 0x01) );
      data >>= 1;
    }
  }

  uint8 get_DATA_BUS() {
    uint8 val = 0;
    for (uint8 bit = DATA_BUS_WIDTH; bit > 0; ) {
      val <<= 1;
      if (digitalRead(DBUS[--bit]) == HIGH) 
        val |= 0x01;
    }
    return val;
  }
};

enum Z80_Mega {
  CLK     = 13, // in
  _INT    = 8,  // in
  _NMI    = 9,  // in
  _HALT   = 10, // out
  _MREQ   = 11, // out
  _IOREQ  = 12, // out 
  _RD     = 40, // out
  _WR     = 41, // out
  _BUSACK = 2,  // out
  _WAIT   = 3,  // in
  _BUSREQ = 4,  // in 
  _RESET  = 5,  // in
  _M1     = 6,  // out
  _RFSH   = 7,  // out
};
const uint8 ADDR_BUS_WIDTH = 16;
const uint8 MEGA_ADDR_BUS[ADDR_BUS_WIDTH] = {
  // PORTF
  54, 55, 56, 57, 58, 59, 60, 61, 
  // PORTK
  62, 63, 64, 65, 66, 67, 68, 69,
};
const uint8 DATA_BUS_WIDTH = 8;
const uint8 MEGA_DATA_BUS[DATA_BUS_WIDTH] = {
  // PORTL
  21, 20, 19, 18, 17, 16, 15, 14,
};

MPU_Z80 z80(CLK, _INT, _NMI, _WAIT, _BUSREQ, _RESET, 
    _MREQ, _RD, _WR, _M1, _RFSH,
    MEGA_ADDR_BUS, MEGA_DATA_BUS);

uint16 addr;
uint8 mem[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void setup() {

  // put your setup code here, to run once:
  Serial.begin(38400);
  while (! Serial) {}
  Serial.println("started.");
  z80.DATA_BUS_mode(OUTPUT);
  z80.set_DATA_BUS(0x00);
  z80.reset();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( z80.clock() == LOW) {
    z80.toggle_clock();
    Serial.print("MREQ = ");
    Serial.print(z80.MREQ(), HEX);
    Serial.print(", RD = ");
    Serial.print(z80.RD(), HEX);
    Serial.print(", WR = ");
    Serial.print(z80.WR(), HEX);
    Serial.print(", M1 = ");
    Serial.print(z80.M1(), HEX);
    Serial.println();
    if ( !z80.MREQ() ) {
      if ( !z80.RD() ) {
        addr = z80.get_ADDR_BUS();
        Serial.print("ABUS = ");
        Serial.println(addr, HEX);
        if (! z80.M1()) {
          Serial.println("OP code fetch");
        }
        z80.DATA_BUS_mode(OUTPUT);
        z80.set_DATA_BUS(0x00); //mem[addr & 0x0f]);
      } else if ( !z80.WR() ) {
        addr = z80.get_ADDR_BUS();
        Serial.print("ABUS = ");
        Serial.println(addr, HEX);
        z80.DATA_BUS_mode(INPUT);
        mem[addr & 0x0f] = z80.get_DATA_BUS();
      }
    }
    Serial.print("DBUS = ");
    Serial.println(z80.get_DATA_BUS(), HEX);
    Serial.println();
    delay(250);
  } else {
    z80.toggle_clock();
    delay(250);
  }
}
