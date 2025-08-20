#ifndef _SRAM1MBIT_H_
#define _SRAM1MBIT_H_

#ifndef uint8 
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#endif

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
    pinMode(EN, OUTPUT);
    disable();
    pinMode(_CS, INPUT);
    deselect();
    pinMode(_OE, INPUT);
    output_disable();
    pinMode(_WE, INPUT);
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

  void select(void) {
    digitalWrite(_CS, LOW);
  }

  void deselect(void) {
    digitalWrite(_CS, HIGH);
  }

  void enable(void) {
    digitalWrite(EN, HIGH);
  }

  void disable(void) {
    digitalWrite(EN, LOW);
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

#endif