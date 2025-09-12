#ifndef _Z80BUS_H_
#define _Z80BUS_H_

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

struct Z80Bus {
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
  const uint8 SRAMEN;

  volatile uint8 * DATA   = PORTL;

  const uint8 CLK_OUT = 13; // PB7 (OC1C)

  const uint8 MEM_EN = 2;

  enum DMA_mode {
    NO_REQUEST = 0,
    READ_RAM  = 1,
    WRITE_RAM = 0xff,
  };
  uint16 dma_address;
  const uint16 dma_block_size = 0x100;
  DMA_mode dma_mode;
  uint8 dma_result;

public:
  Z80Bus(const uint8 & intr, const uint8 & nmi, const uint8 & halt,
            const uint8 & mreq, const uint8 & iorq, const uint8 & rd, const uint8 & wr,
            const uint8 & busack, const uint8 & wait, const uint8 & busreq, 
            const uint8 & reset, const uint8 & m1, const uint8 & rfsh,
            const uint8 & memen) :
  _INT(intr), _NMI(nmi), _HALT(halt), _MREQ(mreq), _IORQ(iorq), 
  _RD(rd), _WR(wr), _BUSACK(busack), _WAIT(wait), 
  _BUSREQ(busreq), _RESET(reset), _M1(m1), _RFSH(rfsh),
  SRAMEN(memen)
  {
    init();
  }

  void init() {
    clock_stop();
  //
    address_bus16_mode(INPUT);
    data_bus_mode_input();
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    pinMode(SRAMEN, OUTPUT);
    ram_enable();

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

    dma_address = 0;
    dma_result = 0;
    dma_mode = NO_REQUEST;
  }

  void clock_start(uint8_t presc, uint16_t top) {
    const uint8_t WGM_CTC_OCR1A = B0100;
    const uint8_t COM_TOGGLE = B01;
    presc = (presc > 5 ? 5 : presc);
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
		if (RESET() == HIGH) {
			if (BUSACK() == HIGH) {
				BUSREQ (LOW);
				while (BUSACK() == HIGH)
					clock_wait_rising_edge();
			}
		}
		ram_disable();
		address_bus16_mode (OUTPUT);
		pinMode(_MREQ, OUTPUT);
		pinMode(_RD, OUTPUT);
		pinMode(_WR, OUTPUT);
		MREQ (HIGH);
		ram_enable();
		return true;
	}

  bool Z80_mode() {
    ram_disable();
    address_bus16_mode(INPUT);
    pinMode(_MREQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    ram_enable();
    if (BUSREQ() == LOW)
      BUSREQ(HIGH);
    if (RESET() == LOW)
      RESET(HIGH);
    while ( BUSACK() == LOW ) ;
  }

  bool memory_mode() {
    ram_disable();
    address_bus16_mode(INPUT);
    pinMode(_MREQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
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
    //digitalWrite(29,(addr>>7) & 0x0001 ? HIGH : LOW);
    // ^ to solve PORTA 7th bit missing problem (reason unknown)
    ADDR_OUT_H = (addr >> 8) & 0xff;
  }

  void data_bus_mode_input() {
    DATA_DIR = 0x00; 
    DATA_OUT = 0x00;
  }

  void data_bus_mode_output() {
    DATA_DIR = 0xff;
  }

  uint8 data_bus_get() {
    return DATA_IN;
  }

  void data_bus_set(uint8 val) {
    DATA_OUT = val;
  }

  void ram_enable() {
    digitalWrite(SRAMEN, HIGH);
  }

  void ram_disable() {
    digitalWrite(SRAMEN, LOW);
  }

  void ram_write(uint16 addr, uint8 data) {
    // ensure the state 
    MREQ(LOW);
    RD(HIGH); // _OE
    WR(HIGH); // _WR
    data_bus_mode_output();
    address_bus16_set(addr);
    //
    data_bus_set(data);
    WR(LOW); // _WE = Low
    __asm__ __volatile("nop"); // can be possibly omitted in write (1/16MHz = 62.5 ns)
    //delayMicroseconds(1); // Wait 1 u sec
    WR(HIGH);
    MREQ(HIGH);
  }

  uint8 ram_read(uint16 addr) {
    uint8 val = 0;
    // ensure
    MREQ(LOW);
    RD(HIGH); // _OE
    WR(HIGH); // _WR
    address_bus16_set(addr);
    data_bus_mode_input();
    //
    RD(LOW);    // _OE = Low, falling edge
    //delayMicroseconds(1); // Wait 1 u sec
    __asm__ __volatile("nop"); //(1/16MHz = 62.5 ns, seems works fine with 55ns chip)
    val = data_bus_get();
    RD(HIGH);
    MREQ(HIGH);
    return val;
  }

  uint8 DMA_progmem_load(const uint8 PROGMEM * mem, const uint16 size, const uint16 & dst_addr) {
    uint16 addr;
    uint8 data;
    uint16 errcount = 0;
    for(uint16 ix = 0; ix < size; ++ix) {
      addr = dst_addr + ix;
      data = pgm_read_byte_near(mem + ix);
      ram_write(addr, data);
    }
    return (errcount > 0xff ? 0xff : (uint8) errcount);
  }

  uint8 DMA_requested() {
    return dma_mode != NO_REQUEST;
  }

  uint8 DMA_direction() {
    return (dma_mode == WRITE_RAM ? OUTPUT : INPUT);
  }

  void DMA_read(uint8 mem[]) {
    dma_mode = READ_RAM; // issue request internaly
    DMA_exec(mem);
  }

  void DMA_write(uint8 mem[]) {
    dma_mode = WRITE_RAM; // issue request internaly
    DMA_exec(mem);
  }

  uint8   DMA_block_size() {
    return dma_block_size;
  }

  void DMA_address(const uint16 & addr) {
    dma_address = addr;
  }

  void DMA_exec(uint8 mem[]) {
    if ( dma_mode == NO_REQUEST )
      return 0x00;
    if ( BUSACK() == HIGH and RESET() == HIGH ) {
      dma_mode = NO_REQUEST;
      dma_result = 0xff;
      return;
    }
    if ( dma_mode == WRITE_RAM ) {
      for(uint16 ix = 0; ix < dma_block_size; ++ix) {
        ram_write(dma_address + ix, mem[ix]);
      }
    } else if ( dma_mode == READ_RAM ) {
      for(uint16 ix = 0; ix < dma_block_size; ++ix) {
        mem[ix] = ram_read(dma_address + ix);
      }
    }
    dma_mode = NO_REQUEST;
    dma_result = 0x00;
  }

  void RESET(uint8 val) {
    pinMode(_RESET, OUTPUT); 
    digitalWrite(_RESET, val);
  }

  uint8 RESET() {
    digitalRead(_RESET);
  }

  void cpu_reset() {
    RESET(LOW);
    clock_wait_rising_edge(5);
  }

  void NMI(uint8 val) {
    pinMode(_NMI, OUTPUT); 
    digitalWrite(_NMI, val);
  }

  uint8 MREQ() {
    return digitalRead(_MREQ);
  }

  uint8 MREQ(uint8 hilo) {
    digitalWrite(_MREQ, hilo);
  }

  uint8 IORQ() {
    return digitalRead(_IORQ);
  }

  uint8 RD() {
    return digitalRead(_RD);
  }

  uint8 RD(uint8 hilo) {
    digitalWrite(_RD, hilo);
  }

  uint8 WR() {
    return digitalRead(_WR);
  }

  uint8 WR(uint8 hilo) {
    digitalWrite(_WR, hilo);
  }

  uint8 M1() {
    return digitalRead(_M1);
  }
  
  uint8 BUSACK() {
    return digitalRead(_BUSACK);
  }
  
  void WAIT(uint8 hilo) {
    pinMode(_WAIT, OUTPUT);
    digitalWrite(_WAIT, hilo);
  }

  uint8 WAIT() {
    return digitalRead(_WAIT);
  }

  uint8 HALT() {
    return digitalRead(_HALT);
  }

  void BUSREQ(uint8 hilo) {
    pinMode(_BUSREQ, OUTPUT);
    digitalWrite(_BUSREQ, hilo);
  }

  uint8 BUSREQ() {
    digitalRead(_BUSREQ);
  }

  bool bus_request() {
    for(uint8 t = 0; t < 8; ++t) {
      clock_wait_rising_edge();
      if ( ! BUSACK() ) return true;
    }
    return false;
  }

  uint8 RFSH() {
    return digitalRead(_RFSH);
  }
  
  uint8 z80io(const uint8 & port, const uint8 & val, const uint8 & inout) {
    switch(port) {
      case 0:  //CON_STS
        if ( inout == INPUT ) {
          return (Serial.available() ? 0xff : 0x00);
        }
        break;
    case 1:  // CON_IN
      if ( inout == INPUT ) {
        return Serial.read();
      }
      break;
    case 2:  // CON_OUT
      if ( inout == OUTPUT )
        Serial.print((char) val);
      return 0;
      break;
    case 16: // track_sel_h
      break; 
    case 17: // track_sel_l
      break;
    case 18: // sector_sel
      break;
    case 20: // dma adr_L
      if ( inout == OUTPUT ) {
        dma_address = (dma_address & 0xff00) | val;
      }
      break;
    case 21: // dma adr_H
      if ( inout == OUTPUT ) {
        dma_address = (dma_address & 0x00ff) | (val<<8);
      }
      break;
    case 22: // exec_dma
      if (inout == INPUT) {
        dma_mode = READ_RAM;
        return 1; // the number of blocks
      } else if (inout == OUTPUT) {
        dma_mode = WRITE_RAM;
      }
      break;
    default: // dma_rs
      if (inout == INPUT ) {
        return dma_result;
      }
      break;
    }
    return 0;
  }
};

#endif
