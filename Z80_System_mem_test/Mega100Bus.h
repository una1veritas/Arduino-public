#ifndef _MEGA100BUS_H_
#define _MEGA100BUS_H_

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

struct Mega100Bus {
public:
  static const uint8 IOADDR_BUS_WIDTH = 8;
  static const uint8 ADDR_BUS_WIDTH = 16;
  static const uint8 DATA_BUS_WIDTH = 8;

  //const uint8 _RD = 40; // PG1 (RD)
  //const uint8 _WR = 41; // PG0 (WR)
  //const uint8 _ALE = 39; // PG2 (ALE)

  const uint8 _INT, _NMI, _WAIT, _BUSREQ, _RESET; // z80 input
  const uint8 _RD, _WR; // z80 out, sram in
  const uint8 _HALT, _MREQ, _IORQ, /* _RD, _WR, */ _BUSACK, _M1, _RFSH; // z80 output
  const uint8 MEMEN;

  volatile uint8 * DATA   = PORTL;

  const uint8 CLK_OUT = 13; // PB7 (OC1C)

  const uint8 MEM_EN = 2;

public:
  Mega100Bus(const uint8 & intr, const uint8 & nmi, const uint8 & halt, 
            const uint8 & mreq, const uint8 & iorq, const uint8 & rd, const uint8 & wr,
            const uint8 & busack, const uint8 & wait, const uint8 & busreq, 
            const uint8 & reset, const uint8 & m1, const uint8 & rfsh,
            const uint8 & memen) :
  _INT(intr), _NMI(nmi), _HALT(halt), _MREQ(mreq), _IORQ(iorq), 
  _RD(rd), _WR(wr), _BUSACK(busack), _WAIT(wait), 
  _BUSREQ(busreq), _RESET(reset), _M1(m1), _RFSH(rfsh),
  MEMEN(memen)
  {
    init();
  }

  void init() {
    clock_stop();
  //
    address_bus16_mode(INPUT);
    data_bus_mode(INPUT);
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    pinMode(MEMEN, OUTPUT);
    mem_enable();

  // z80 inputs, temporary set input mode.
    pinMode(_INT, INPUT);
    pinMode(_NMI, INPUT);
    pinMode(_MREQ, INPUT);
    pinMode(_IORQ, INPUT);
    pinMode(_WAIT, INPUT);
    pinMode(_BUSREQ, INPUT);
    pinMode(_RESET, INPUT); 
    // z80 output port, fixed to input mode.
    pinMode(_HALT, INPUT);
    pinMode(_BUSACK, INPUT);
    pinMode(_M1, INPUT); 
    pinMode(_RFSH, INPUT); 
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

  bool DMA_mode() {
    if ( BUSACK() == LOW ) {
      mem_disable();
      address_bus16_mode(OUTPUT);
      pinMode(_MREQ, OUTPUT);
      pinMode(_RD, OUTPUT);
      pinMode(_WR, OUTPUT);
      mem_deselect();
      mem_enable();
      return true;
    } else
    return false;
  }

  bool Z80_mode() {
    mem_disable();
    address_bus16_mode(INPUT);
    pinMode(_MREQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    mem_deselect();
    mem_enable();
    BUSREQ(HIGH);
    while ( BUSACK() == LOW ) ;
  }

  void address_bus16_mode(uint8 in_out) {
    if (in_out == INPUT) {
      ADDR_DIR_L = 0x00; ADDR_OUT_L = 0;
      ADDR_DIR_H = 0x00; ADDR_OUT_H = 0;
    } else {
      ADDR_DIR_L = 0xff;
      ADDR_DIR_H = 0xff;
    }
  }

  uint16 address_bus16_get() {
    uint16 val;
    val = ADDR_IN_L | (ADDR_IN_H << 8);
    return val;
  }

  void address_bus16_set(uint16 addr) {
    ADDR_OUT_L = addr & 0xff;
    ADDR_OUT_H = (addr >> 8) & 0xff;
  }

  void data_bus_mode(uint8 in_out) {
    if (in_out == INPUT) {
      DATA_DIR = 0x00; DATA_OUT = 0x00;
    } else {
      DATA_DIR = 0xff; 
    }
  }

  uint8 data_bus_get() {
    return DATA_IN;
  }

  void data_bus_set(uint8 val) {
    DATA_OUT = val;
  }

  void mem_enable() {
    digitalWrite(MEMEN, HIGH);
  }

  void mem_disable() {
    digitalWrite(MEMEN, LOW);
  }

  void mem_select() {
    digitalWrite(_MREQ, LOW);
  }

  void mem_deselect() {
    digitalWrite(_MREQ, HIGH);
  }

  void mem_mode_non_read() {
    digitalWrite(_RD, HIGH);
  }

  void mem_mode_non_write() {
    digitalWrite(_WR, HIGH);
  }

  void mem_mode_none() {
    digitalWrite(_RD, HIGH);
    digitalWrite(_WR, HIGH);
  }

  void mem_mode_read() {
    digitalWrite(_RD, LOW);
  }
  void mem_mode_write() {
    digitalWrite(_WR, LOW);
  }

  void mem_write(uint16 addr, uint8 data) {
    // ensure the state
    mem_mode_none();
    address_bus16_mode(OUTPUT);
    mem_select();
    address_bus16_set(addr);
    //
    data_bus_mode(OUTPUT);
    data_bus_set(data);
    mem_mode_write(); // _WE = Low
    __asm__ __volatile("nop"); // can be possibly omitted in write (1/16MHz = 62.5 ns)
    //delayMicroseconds(1); // Wait 1 u sec
    mem_mode_non_write();
  }

  uint8 mem_read(uint16 addr) {
    uint8 val = 0;
    mem_mode_none();
    address_bus16_mode(OUTPUT);
    mem_select();
    address_bus16_set(addr);
    //
    data_bus_mode(INPUT);
    mem_mode_read();    // _OE = Low, falling edge
    //delayMicroseconds(1); // Wait 1 u sec
    __asm__ __volatile("nop"); //(1/16MHz = 62.5 ns, seems works fine with 55ns chip)
    val = data_bus_get();
    mem_mode_non_read();
    return val;
  }


  void Z80_RESET(uint8 val) {
    pinMode(_RESET, OUTPUT); 
    digitalWrite(_RESET, val);
  }

  void Z80_NMI(uint8 val) {
    pinMode(_NMI, OUTPUT); 
    digitalWrite(_NMI, val);
  }

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

  void BUSREQ(uint8 val) {
    pinMode(_BUSREQ, OUTPUT);
    digitalWrite(_BUSREQ, val);
  }


};

#endif
