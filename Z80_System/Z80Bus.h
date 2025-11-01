#ifndef _Z80BUS_H_
#define _Z80BUS_H_

#include <Arduino.h>
#include <SD.h>

#include "Z80Bus_DMA_FDC.h"
#include "Z80Bus_SIO.h"

#define ADDR_OUT_L PORTA
#define ADDR_OUT_H PORTC
#define ADDR_IN_L PINA
#define ADDR_IN_H PINC
#define ADDR_DIR_L DDRA
#define ADDR_DIR_H DDRC
#define DATA_DIR DDRL
#define DATA_IN PINL
#define DATA_OUT PORTL


enum IO_Ports {
    CONSTA = 0,  //console status port
    CONDAT = 1,      // CONIN, console data in/out port
	CONIO = 1,
	CONOUT = 2,
	CONSCAN = 3,
    // PRTSTA,
    // PRTDAT,
    // AUXDAT = 7,		// aux data port
    // FDCD = 8,  	//fdc-port:
    FDCDRIVE = 10,  //fdc-port: # of drive
    FDCTRACK,       //fdc-port: # of track
    FDCSECTOR,      //fdc-port: # of sector
    FDCOP,       //fdc-port: command
    FDCST,       //fdc-port: status
    DMAL = 15,   //dma-port: dma address low
    DMAH,        //dma-port: dma address high
	DMABLKSIZE,
	DMAEXEC,
	DMARES,

	XSTREAMST = 24, 	// aux stream I/O status
	XSTREAMDAT, 		// aux stream I/O

	CLKMODE = 128,
	LED7SEG  = 129,

	FILECHG = 135, // re-mount SD
};


struct Z80Bus {
public:
  static const uint8_t IOADDR_BUS_WIDTH = 8;
  static const uint8_t ADDR_BUS_WIDTH = 16;
  static const uint8_t DATA_BUS_WIDTH = 8;

  static const uint8_t boot_0000[256] PROGMEM;
  static const uint8_t mon_0100[768] PROGMEM;
  static const uint8_t basic_0000[8192] PROGMEM;

public:
  //const uint8_t_RD = 40; // PG1 (RD)
  //const uint8_t_WR = 41; // PG0 (WR)
  //const uint8_t_ALE = 39; // PG2 (ALE)

  const uint8_t _INT, _NMI, _WAIT, _BUSREQ, _RESET;                        // z80 input
  const uint8_t _RD, _WR;                                                  // z80 out, sram in
  const uint8_t _HALT, _MREQ, _IORQ, /* _RD, _WR, */ _BUSACK, _M1, _RFSH;  // z80 output
  const uint8_t SRAM_EN;
  //volatile uint8_t * DATA   = PORTL;
  const uint8_t CLK_OUT = 13;  // PB7 (OC1C)

  const uint8_t MEM_EN = 2;

  Disk_Controller fdc;
  DMA_Controller dma;

  const char registered_filepath[4][32] = {
		  "/DSK/drivea.dsk",
		  "/EMUZ80BASIC.hex",
		  "",
		  "",
  };
  
  uint8_t clock_mode;
  //uint8_t bus_mode;

  enum MEMORY_BUS_MODE {
	  Z80_THRU_RW_MODE 	= 0x00,
	  Z80_MMU_RW_MODE 	= 0x02,
	  MEGA_RW_MODE 		= 0x04,
  };

  uint8_t PROGMEM * pages[16]{
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

public:
  Z80Bus(const uint8_t& intr, const uint8_t& nmi, const uint8_t& halt,
         const uint8_t& mreq, const uint8_t& iorq, const uint8_t& rd, const uint8_t& wr,
         const uint8_t& busack, const uint8_t& wait, const uint8_t& busreq,
         const uint8_t& reset, const uint8_t& m1, const uint8_t& rfsh,
         const uint8_t& memen)
    : _INT(intr), _NMI(nmi), _HALT(halt), _MREQ(mreq), _IORQ(iorq),
      _RD(rd), _WR(wr), _BUSACK(busack), _WAIT(wait),
      _BUSREQ(busreq), _RESET(reset), _M1(m1), _RFSH(rfsh),
      SRAM_EN(memen) {
    init();
  }

  void init() {
    clock_mode_select(3); // default
    clock_stop();
    //
    address_bus16_mode(INPUT);
    data_bus_mode_input();
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    pinMode(SRAM_EN, OUTPUT);
    ram_enable();

    // z80/memory inputs, temporary set input mode.
    pinMode(_MREQ, INPUT);
    pinMode(_IORQ, INPUT);
    // z80 inputs, fixed to input mode.
    pinMode(_INT, OUTPUT);
    digitalWrite(_INT, HIGH);
    pinMode(_NMI, OUTPUT);
    digitalWrite(_NMI, HIGH);
    pinMode(_WAIT, OUTPUT);
    digitalWrite(_WAIT, HIGH);
    pinMode(_BUSREQ, OUTPUT);
    digitalWrite(_BUSREQ, HIGH);
    pinMode(_RESET, OUTPUT);
    digitalWrite(_RESET, HIGH);

    // z80 output port, fixed to input mode.
    pinMode(_HALT, INPUT);
    pinMode(_BUSACK, INPUT);
    pinMode(_M1, INPUT);
    pinMode(_RFSH, INPUT);

    dma.init();
    fdc.init();
    usart1_init(57600);
    //set_rom_page(rom_mon_F000, 0x0f);
  }

  void set_rom_page(const uint8_t* rom PROGMEM, const uint8_t pageindex) {
    pages[pageindex] = rom;
  }

  void clock_set(uint8_t presc, uint16_t top);
  /* disable interrupt, set up TTCR1 control registers and activate timer 1 counter/toggle mode,
   * set prescaler, counter top value, enable intewrrupt,
   * and set CLK_OUT pin mode to output.
   */

  void clock_mode_select(const uint8_t mode);
  uint8_t clock_mode_current() { return clock_mode; }
  void clock_start();
  void clock_stop();
  /* disconnect ocra1 and stop WGM (b10),
   * set CLK_OUT to input pulluped.
   */

  bool clock_is_running() {
    return (TCCR1A & (B11 << COM1C0)) == (B01 << COM1C0);
  }

  uint8_t clock() {
    return digitalRead(CLK_OUT);
  }

  void clock_wait_rising_edge(uint8_t t = 1) {
    while (t > 0) {
      while (digitalRead(CLK_OUT) == HIGH)
        ;
      --t;
      while (digitalRead(CLK_OUT) == LOW)
        ;
    }
  }

  /*
	 * In this mode atmegaXXX00 controls all the bus
	 * with MREQ/EN, IORQ/IO, RD/OE, WR/WE by stopping Z80
	 * in BUSREQ/BUSACK state, or RESET and clock stop.
	 * ATMega controls all the devices.
	 */
  bool mem_bus_DMA_mode() {
    if (RESET() == HIGH) {
      if (BUSACK() == HIGH) {
        BUSREQ(LOW);
        while (BUSACK() == HIGH)
          clock_wait_rising_edge();
      }
    }
    ram_disable();
    address_bus16_mode(OUTPUT);
    pinMode(_MREQ, OUTPUT);
    pinMode(_RD, OUTPUT);
    pinMode(_WR, OUTPUT);
    MREQ(HIGH);
    ram_enable();
    return true;
  }

  /*
	 * Memory Manager/Controller mode.
	 * ATmega does MMU function and
	 * controls SRAM access, ROM emulation,
	 * as well as the memory page control.
	 */
  bool mem_bus_Z80_mode() {
    ram_disable();  // ram/rom is controlled by atmega100
    address_bus16_mode(INPUT);
    data_bus_mode_input();
    pinMode(_MREQ, INPUT);
    pinMode(_RD, INPUT);
    pinMode(_WR, INPUT);
    ram_enable();
    if (BUSREQ() == LOW)
      BUSREQ(HIGH);
    if (RESET() == LOW)
      RESET(HIGH);
    while (BUSACK() == LOW)
      ;
  }

  void address_bus16_mode(uint8_t in_out) {
    if (in_out == INPUT) {
      ADDR_DIR_L = 0x00;
      ADDR_OUT_L = 0;
      ADDR_DIR_H = 0x00;
      ADDR_OUT_H = 0;
    } else {
      ADDR_DIR_L = 0xff;
      ADDR_DIR_H = 0xff;
    }
  }

  uint16_t address_bus16_get() {
	uint16_t word;
    ((uint8_t *) & word)[0] = ADDR_IN_L;
    ((uint8_t *) & word)[1] = ADDR_IN_H;
    return word;
  }

  void address_bus16_set(uint16_t addr) {
    ADDR_OUT_L = (uint8_t) addr;
    //digitalWrite(29,(addr>>7) & 0x0001 ? HIGH : LOW);
    // ^ to solve PORTA 7th bit missing problem (reason unknown)
    ADDR_OUT_H = (uint8_t) (addr >> 8);
  }

  void data_bus_mode_input() {
    DATA_DIR = 0x00;
    DATA_OUT = 0x00;
  }

  void data_bus_mode_output() {
    DATA_DIR = 0xff;
  }

  uint8_t data_bus_get() {
    return DATA_IN;
  }

  void data_bus_set(uint8_t val) {
    DATA_OUT = val;
  }

  void ram_enable() {
    digitalWrite(SRAM_EN, HIGH);
  }

  void ram_disable() {
    digitalWrite(SRAM_EN, LOW);
  }

  bool ram_is_enabled() {
    return digitalRead(SRAM_EN) == HIGH;
  }

  uint16_t ram_check(const uint32_t& start_addr = 0x0000, const uint32_t& end_addr = 0x10000);

  void ram_write(uint16_t addr, uint8_t data) {
    // ensure the state
    MREQ(LOW);
    RD(HIGH);  // _OE
    WR(HIGH);  // _WR
    data_bus_mode_output();
    address_bus16_set(addr);
    //
    data_bus_set(data);
    WR(LOW);                    // _WE = Low
    __asm__ __volatile("nop");  // can be possibly omitted in write (1/16MHz = 62.5 ns)
    //delayMicroseconds(1); // Wait 1 u sec
    WR(HIGH);
    MREQ(HIGH);
  }

  uint8_t ram_read(uint16_t addr) {
    uint8_t val = 0;
    // ensure
    MREQ(LOW);
    RD(HIGH);  // _OE
    WR(HIGH);  // _WR
    address_bus16_set(addr);
    data_bus_mode_input();
    //
    RD(LOW);  // _OE = Low, falling edge
    //delayMicroseconds(1); // Wait 1 u sec
    __asm__ __volatile("nop");  //(1/16MHz = 62.5 ns, seems works fine with 55ns chip)
    val = data_bus_get();
    RD(HIGH);
    MREQ(HIGH);
    return val;
  }

  uint8_t DMA_progmem_load(const uint8_t PROGMEM* mem, const uint16_t& dst_addr, const uint16_t size) {
    uint16_t addr;
    uint8_t data;
    uint16_t errcount = 0;
    for (uint16_t ix = 0; ix < size; ++ix) {
      addr = dst_addr + ix;
      data = pgm_read_byte_far(mem + ix);
      ram_write(addr, data);
    }
    return (errcount > 0xff ? 0xff : (uint8_t)errcount);
  }

  void DMA_exec(uint8_t mem[]) {
    if (dma.transfer_mode == dma.NO_REQUEST)
      dma.result = dma.TRANSFER_OK;
    if ( BUSACK() == HIGH ) {
      dma.transfer_mode = dma.NO_REQUEST;
      dma.result = 0xff;
      return;
    }
    //ram_enable();
    if (dma.transfer_mode == dma.FROM_BUFFER_TO_RAM) {
      for (uint16_t ix = 0; ix < dma.block_size(); ++ix) {
        ram_write(dma.target_address() + ix, mem[ix]);
      }
    } else if (dma.transfer_mode == dma.FROM_RAM_TO_BUFFER) {
      for (uint16_t ix = 0; ix < dma.block_size(); ++ix) {
        mem[ix] = ram_read(dma.target_address() + ix);
      }
    }
    //ram_disable();
    dma.transfer_mode = dma.NO_REQUEST;
    dma.result = dma.TRANSFER_OK;
  }

	void FDC_operate(uint8_t buffer[]) {
		int res;
		unsigned long pos = fdc.seek_position();
		unsigned long nbytes;
		if ( fdc.drive().dskfile.size() <= pos ) {
			fdc.drive().status = fdc.RES_ERROR;
			return;
		}
		nbytes = fdc.drive().dtype.sector_size;
		if ( fdc.drive().dskfile.size() < pos + nbytes ) {
			nbytes = fdc.drive().dskfile.size() - pos;
		}
		fdc.drive().dskfile.seek(pos);
		if (fdc.opcode == fdc.READ_SECTOR) {
			if ( fdc.drive().dskfile.read(buffer, nbytes) < 0 )
				fdc.drive().status = fdc.RES_ERROR;
		} else if (fdc.opcode == fdc.WRITE_SECTOR) {
			if ( fdc.drive().dskfile.write(buffer, nbytes) < 0 )
				fdc.drive().status = fdc.RES_ERROR;
		}
		fdc.opcode = fdc.NO_REQUEST;
	}

  void RESET(uint8_t val) {
    pinMode(_RESET, OUTPUT);
    digitalWrite(_RESET, val);
  }

  uint8_t RESET() {
    digitalRead(_RESET);
  }

  void cpu_reset() {
    RESET(LOW);
    clock_wait_rising_edge(5);
  }

  void NMI(uint8_t val) {
    pinMode(_NMI, OUTPUT);
    digitalWrite(_NMI, val);
  }

  uint8_t MREQ() {
    return digitalRead(_MREQ);
  }

  uint8_t MREQ(uint8_t hilo) {
    digitalWrite(_MREQ, hilo);
  }

  uint8_t IORQ() {
    return digitalRead(_IORQ);
  }

  uint8_t RD() {
    return digitalRead(_RD);
  }

  uint8_t RD(uint8_t hilo) {
    digitalWrite(_RD, hilo);
  }

  uint8_t WR() {
    return digitalRead(_WR);
  }

  uint8_t WR(uint8_t hilo) {
    digitalWrite(_WR, hilo);
  }

  uint8_t M1() {
    return digitalRead(_M1);
  }

  uint8_t BUSACK() {
    return digitalRead(_BUSACK);
  }

  void WAIT(uint8_t hilo) {
    digitalWrite(_WAIT, hilo);
  }

  uint8_t WAIT() {
    return digitalRead(_WAIT);
  }

  uint8_t HALT() {
    return digitalRead(_HALT);
  }

  void BUSREQ(uint8_t hilo) {
    pinMode(_BUSREQ, OUTPUT);
    digitalWrite(_BUSREQ, hilo);
  }

  uint8_t BUSREQ() {
    return digitalRead(_BUSREQ);
  }

  bool bus_request() {
    for (uint8_t t = 0; t < 8; ++t) {
      clock_wait_rising_edge();
      if (!BUSACK()) return true;
    }
    return false;
  }

  uint8_t RFSH() {
    return digitalRead(_RFSH);
  }

  //  uint8_t io_rw(const uint8_t & port, const uint8_t & val, const uint8_t & inout);
  void io_rw(void);
  void emulate_rom(void);
  uint8_t ascii7seg(char);
};

#endif
