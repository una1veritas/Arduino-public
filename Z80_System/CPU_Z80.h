#ifndef _CPU_Z80_H_
#define _CPU_Z80_H_

#ifndef uint8 
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#endif

class CPU_Z80 {
private:
  static const uint8 IOADDR_BUS_WIDTH = 8;
  static const uint8 ADDR_BUS_WIDTH = 16;
  static const uint8 DATA_BUS_WIDTH = 8;
  //
  const uint8 _CLK, _INT, _NMI, _WAIT, _BUSREQ, _RESET; // input
  const uint8 _HALT, _MREQ, _IORQ, _RD, _WR, _BUSACK, _M1, _RFSH; //__output
  const uint8 * ABUS;
  const uint8 * DBUS;

public:

  CPU_Z80(const uint8 clk, const uint8 _int, const uint8 _nmi, const uint8 _halt, const uint8 _mreq, const uint8 _iorq,     
    const uint8 _rd, const uint8 _wr, const uint8 _busack, const uint8 _wait, const uint8 _busreq, const uint8 _rst, const uint8 _m1, const uint8 _rfsh,
    const uint8 addr_bus[], const uint8 data_bus[]) 
  : _CLK(clk), _INT(_int), _NMI(_nmi), _HALT(_halt), _MREQ(_mreq), _IORQ(_iorq), _RD(_rd), _WR(_wr), _BUSACK(_busack), _WAIT(_wait), _BUSREQ(_busreq), _RESET(_rst), _M1(_m1), _RFSH(_rfsh), 
  ABUS(addr_bus), DBUS(data_bus) {
    init();
  }

  void init() {
    pinMode(_CLK, OUTPUT);
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

    pinMode(_HALT, INPUT);
    pinMode(_MREQ, INPUT);
    pinMode(_IORQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_BUSACK, INPUT);
    pinMode(_M1, INPUT);
    pinMode(_RFSH, INPUT);

    for(uint8 pix = 0; pix < ADDR_BUS_WIDTH; ++pix) {
      pinMode(ABUS[pix], INPUT);
      digitalWrite(ABUS[pix], HIGH);
    }
    DATA_BUS_mode(INPUT);
  }

  uint8 control_pin_status(){
    uint8 s = 0;
    s |= (digitalRead(_HALT) == HIGH ? 1 : 0) << 0;
    s |= (digitalRead(_MREQ) == HIGH ? 1 : 0) << 6;
    s |= (digitalRead(_IORQ) == HIGH ? 1 : 0) << 3;
    s |= (digitalRead(_RD) == HIGH ? 1 : 0) << 4;
    s |= (digitalRead(_WR) == HIGH ? 1 : 0) << 5;
    s |= (digitalRead(_BUSACK) == HIGH ? 1 : 0) << 7;
    s |= (digitalRead(_M1) == HIGH ? 1 : 0) << 2;
    s |= (digitalRead(_RFSH) == HIGH ? 1 : 0) << 1;
    return s;
  }

  void RESET(uint8 val) {
    digitalWrite(_RESET, val);
  }
  inline void RESET_LO() { RESET(LOW); }
  inline void RESET_HI() { RESET(HIGH); }

  uint8 CLK() {
    return digitalRead(_CLK);
  }

  uint8 HALT() {
    return digitalRead(_HALT);
  }

  uint8 MREQ() {
    return digitalRead(_MREQ);
  }

  uint8 IORQ() {
    return digitalRead(_IORQ);
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
  
  uint8 BUSACK() {
    return digitalRead(_M1);
  }
  
  void WAIT(uint8 val) {
    digitalWrite(_WAIT, val);
  }
  inline void WAIT_LO() {
	  WAIT(LOW);
  }
  inline void WAIT_HI() {
	  WAIT(HIGH);
  }

  void BUSREQ(uint8 val) {
    digitalWrite(_BUSREQ, val);
  }
  inline void BUSREQ_LO() {
	  BUSREQ(LOW);
  }
  inline void BUSREQ_HI() {
	  BUSREQ(HIGH);
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

#endif
