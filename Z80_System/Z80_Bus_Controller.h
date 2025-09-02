#ifndef _Z80_BUS_CONTROLLER_H_
#define _Z80_BUS_CONTROLLER_H_

#ifndef uint8 
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#endif

#define ADDR_OUT_L PORTA
#define ADDR_OUT_H PORTC
#define ADDR_IN_L  PINA
#define ADDR_IN_H  PINC
#define ADDR_DIR_L DDRA
#define ADDR_DIR_H DDRC
#define DATA_DIR DDRL
#define DATA_IN  PINL
#define DATA_OUT PORTL

struct Z80_Bus_Controller {
public:
  static const uint8 IOADDR_BUS_WIDTH = 8;
  static const uint8 ADDR_BUS_WIDTH = 16;
  static const uint8 DATA_BUS_WIDTH = 8;

  const uint8 _RD = 40; // PG1 (RD)
  const uint8 _WR = 41; // PG0 (WR)
  const uint8 _ALE = 39; // PG2 (ALE)

  const uint8 _INT, _NMI, _WAIT, _BUSREQ, _RESET; // input
  const uint8 _HALT, _MREQ, _IORQ, /* _RD, _WR, */ _BUSACK, _M1; //, _RFSH; __output

  volatile uint8 * DATA   = PORTL;

  const uint8 CLK_OUT = 13; // PB7 (OC1C)

/*
  const uint8 ADDR_BUS_OE = 39;
  const uint8 DATA_BUS_OE = 38;
  const uint8 ADDR_BUS_DIR = 4;
  const uint8 DATA_BUS_DIR = 5;
*/
  const uint8 MEM_EN = 2;

public:
/*
  Z80_Bus_Controller(const uint8 _int, const uint8 _nmi, const uint8 _halt, const uint8 _mreq, const uint8 _iorq,
    const uint8 _rd, const uint8 _wr, const uint8 _busack, const uint8 _wait, const uint8 _busreq, const uint8 _rst, const uint8 _m1, const uint8 _rfsh,
    const uint8 addr_bus[], const uint8 data_bus[]) 
  : _INT(_int), _NMI(_nmi), _HALT(_halt), _MREQ(_mreq), _IORQ(_iorq), _RD(_rd), _WR(_wr), _BUSACK(_busack), _WAIT(_wait), _BUSREQ(_busreq), _RESET(_rst), _M1(_m1), _RFSH(_rfsh),
  ABUS(addr_bus), DBUS(data_bus) {
    init();
  }
*/
  Z80_Bus_Controller(void) :
  _INT(12), _NMI(11), _HALT(10), _MREQ(9), _IORQ(8), 
  /* _RD(_rd), _WR(_wr),*/ _BUSACK(7), _WAIT(6), 
  _BUSREQ(5), _RESET(4), _M1(3) /*, _RFSH(2) */
  {
    reset();
  }

  void reset() {
    clock_stop();

    listen_address_bus16();
    listen_data_bus();    
    memory_enable();

    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    pinMode(MEM_EN, INPUT);

  // inputs
    pinMode(_INT, INPUT);
    pinMode(_NMI, INPUT);
    pinMode(_WAIT, INPUT);
    pinMode(_BUSREQ, INPUT);
    pinMode(_RESET, INPUT); 
    // outputs
    pinMode(_HALT, INPUT);
    pinMode(_MREQ, INPUT);
    pinMode(_IORQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    pinMode(_BUSACK, INPUT);
    pinMode(_M1, INPUT); 
  }

  void clock_start(uint8_t presc, uint16_t top) {
    const uint8_t WGM_CTC_OCR1A = B0100;
    const uint8_t COM_TOGGLE = B01;

    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;
    TCNT1 = 0;
    OCR1A = top - 1;

    TCCR1A |= (COM_TOGGLE << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);
    TCCR1B |= (((WGM_CTC_OCR1A>>2) & B11)<< WGM12)  | (presc << CS10);

    sei();

    pinMode(CLK_OUT, OUTPUT);
  }

  void clock_stop() {
    const uint8_t WGM_CTC_OCR1A = B0100;
    const uint8_t COM_DISCONNECT = B00;

    cli();

    TCCR1A |= (COM_DISCONNECT << COM1C0) | ((WGM_CTC_OCR1A & B0011) << WGM10);

    sei();

    pinMode(CLK_OUT, INPUT_PULLUP);
  }

  bool clock_is_running() {
	  return (TCCR1A & (B11 << COM1C0)) == (B01 << COM1C0);
  }

  uint8 clock() {
    return digitalRead(CLK_OUT);
  }

  void clock_wait_rising_edge(uint8 t = 1) {
    while ( t > 0 ) {
      while ( digitalRead(CLK_OUT) == HIGH);
      --t;
      while ( digitalRead(CLK_OUT) == LOW);
    }
  }

  void memory_enable() {
    pinMode(MEM_EN, OUTPUT);
    digitalWrite(MEM_EN, HIGH);
  }

  void memory_disable() {
    pinMode(MEM_EN, OUTPUT);
    digitalWrite(MEM_EN, HIGH);
  }
/*
  uint8 pin_status(){
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
*/

  void Z80_RESET(uint8 val) {
    pinMode(_RESET, OUTPUT); 
    digitalWrite(_RESET, val);
  }

  void Z80_NMI(uint8 val) {
    pinMode(_NMI, OUTPUT); 
    digitalWrite(_NMI, val);
  }

/*
  void do_reset() {
    if ( !clock_is_running() )
    return;
	  uint8 count = 0;
	  RESET(LOW);
	  while (count < 5) {
	    while (CLK() == HIGH) ;
	    while (CLK() == LOW) ;
	    ++count;
	  }
	  RESET(HIGH);
  }

  uint8 CLK() {
    return digitalRead(CLK_OC1C);
  }

  uint8 HALT() {
    return digitalRead(_HALT);
  }
*/
  uint8 MREQ() {
    pinMode(_MREQ, INPUT);
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
    return digitalRead(_BUSACK);
  }
  
  void WAIT(uint8 val) {
    digitalWrite(_WAIT, val);
  }
  /*
  inline void WAIT_LO() {
	  WAIT(LOW);
  }
  inline void WAIT_HI() {
	  WAIT(HIGH);
  }
*/
  void BUSREQ(uint8 val) {
    pinMode(_BUSREQ, OUTPUT);
    digitalWrite(_BUSREQ, val);
  }
  /*
  inline void BUSREQ_LO() {
	  BUSREQ(LOW);
  }
  inline void BUSREQ_HI() {
	  BUSREQ(HIGH);
  }
  */
  void listen_address_bus16() {
    ADDR_DIR_L = 0x00; ADDR_OUT_L = 0;
    ADDR_DIR_H = 0x00; ADDR_OUT_H = 0;
  }

  uint16 get_address_bus16() {
    uint16 val;
    val = ADDR_IN_L | (ADDR_IN_H << 8);
    return val;
  }
  void listen_data_bus() {
    DATA_DIR = 0x00; DATA_OUT = 0x00;
  }

  uint8 get_data_bus() {
    return DATA_IN;
  }
/*
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
*/
  /*
    uint8 get_DATA_BUS() {
    uint8 val = 0;
    for (uint8 bit = DATA_BUS_WIDTH; bit > 0; ) {
      val <<= 1;
      if (digitalRead(DBUS[--bit]) == HIGH) 
        val |= 0x01;
    }
    return val;
  }
*/
};

#endif
