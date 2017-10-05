#include "Z80_pindef.h"
#include "Z80.h"
#include "sram.h"

void clock_start(uint8_t presc, uint16_t top);
void clock_enable();
void clock_disable();

void atmega_sramcontrol_mode(uint8_t);
void atmega_addrbus_mode(uint8_t);
void atmega_databus_mode(uint8_t);
uint16_t addrbus(void);
uint8_t databus(void);
uint8_t databus_write(uint8_t);

char * opcode(uint32_t code, char * buf);

const uint16_t mem_size = 256;
uint8_t mem[mem_size] = {
0x31, 0xff, 0x00, 0x21, 0x18, 0x00, 0x06, 0x00, 
0x78, 0x3c, 0x32, 0x27, 0x00, 0x47, 0x7e, 0xa7, 
0x28, 0x05, 0xd3, 0x01, 0x23, 0x18, 0xf1, 0x76, 
0x48, 0x69, 0x2e, 0x20, 0x49, 0x27, 0x6d, 0x20, 
0x5a, 0x38, 0x30, 0x2e, 0x0a, 0x0d, 0x00, 0x17, 
};


char * hexstr(uint32_t val, uint8_t digits) {
  static char buf[8+1];
  digits = digits >= 8 ? 8 : digits;
  for(uint8_t d = digits; d > 0; --d ) {
    buf[d-1] = (val & 0x0f);
    if ( buf[d-1] < 0x0a ) {
      buf[d-1] += '0';
    } else {
      buf[d-1] += ('A' - 10);
    }
    val >>= 4;
  }
  buf[digits] = 0;
  return buf;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi.");

  sram_bus_init();
  sram_bank(0);
  sram_disable();

  atmega_sramcontrol_mode(INPUT);
  atmega_addrbus_mode(INPUT);
  atmega_databus_mode(INPUT);
  
  Serial.println("init Z80 and inactivate.");
  clock_start(5, 800);
  clock_enable();
  Z80_init();
  Z80_reset();
  Serial.println("resetted.");
  Z80_bus_activate();
  Z80_bus_request();
  Serial.println("bus requested.");


  Serial.println("write sram.");
  sram_bus_init(); // also enables
  sram_select();
  for(uint16_t i = 0; i < 256; i++) {
    sram_write(i,mem[i]);
  }
  for(uint16_t i = 0; i < 256; i++) {
    Serial.print(hexstr(sram_read(i),2));
    Serial.print(" ");
    if ( (i & 0x0f) == 0x0f )
      Serial.println();
  }
  Serial.println();
  sram_deselect();
  sram_disable();
  atmega_sramcontrol_mode(INPUT);
  atmega_addrbus_mode(INPUT);
  atmega_databus_mode(INPUT);

  Serial.println("Z80 reset.");
  Z80_bus_activate();
  Z80_reset();

}

void loop() {
  uint16_t addr;
  uint8_t data;
  char buf[16];
  static uint8_t m1;

  if ( Z80_mreq_read() ) {
    atmega_databus_mode(OUTPUT);
    m1 = Z80_m1();
    addr = addrbus();
    data = mem[addr & 0xff];
    databus_write(data);
    while ( Z80_mreq_read() ) {
    }
    atmega_databus_mode(INPUT);
    Serial.print(hexstr(addr, 4));
    if ( m1 ) {
      Serial.print(" M  ");
    } else {
      Serial.print(" R  ");
    }
    Serial.print(hexstr(data,2));
    if ( m1 ) {
      Serial.print(" ");
      opcode(data,buf);
      Serial.print((int) buf[0]);
      Serial.print(" ");
      Serial.print(buf+1);
    }
    Serial.println();
  } else if ( Z80_mreq_write() ) {
    atmega_databus_mode(INPUT);
    addr = addrbus();
    data = databus();
    mem[addr & 0xff] = data;
    while ( Z80_mreq_write() ) {
    }
    Serial.print(hexstr(addr, 4));
    Serial.print("  W ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_iorq_read() ) {
    Z80_wait_request();
    atmega_databus_mode(OUTPUT);
    addr = addrbus();
    data = databus_write(0xaa);
    Z80_wait_cancel();
    while ( Z80_iorq_read() );
    atmega_databus_mode(INPUT);
    Serial.print(hexstr(addr, 4));
    Serial.print(" I  ");
    Serial.print(hexstr(data,2));
    Serial.println();
  } else if ( Z80_iorq_write() ) {
    atmega_databus_mode(INPUT);
    Z80_wait_request();
    addr = addrbus();
    data = databus();
    Z80_wait_cancel();
    while ( Z80_iorq_write() );
    Serial.print(hexstr(addr, 4));
    Serial.print("  O ");
    Serial.print(hexstr(data,2));
    if ( isprint(data) ) {
      Serial.print(" '");
      Serial.print((char)data);
      Serial.print('\'');
    }
    Serial.println();    
  } else if ( !digitalRead(Z80_HALT_PIN) ) {
    Serial.println("halted.");
    clock_disable();
    while ( !digitalRead(Z80_HALT_PIN) );
    clock_enable();
  }
}

void clock_start(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}

void clock_disable() {
  pinMode(Z80_CLK_PIN, INPUT);
  digitalWrite(Z80_CLK_PIN, HIGH);
}

void clock_enable() {
  pinMode(Z80_CLK_PIN, OUTPUT);
}

void atmega_sramcontrol_mode(uint8_t inout) {
  pinMode(SRAM_CS_PIN, inout); // Z80_MREQ
  pinMode(SRAM_OE_PIN, inout); // Z80_RD
  pinMode(SRAM_WE_PIN, inout); // Z80_WR
}

void atmega_addrbus_mode(uint8_t inout) {
  if ( inout == OUTPUT ) {
    DDRA = 0xff;
    DDRC = 0xff;
  } else {
    DDRA = 0x00;
    DDRC = 0x00;    
  }
}

void atmega_databus_mode(uint8_t inout) {
  if ( inout == OUTPUT ) {
    DDRF = 0xff;
  } else {
    DDRF = 0x00;
  }
}

uint16_t addrbus(void) {
  return ((uint16_t) PINC)<<8 | PINA;
}

uint16_t addrbus_write(uint16_t addr) {
  PORTA = addr & 0xff;
  PORTC = addr>>8 & 0xff;
  return addr;
}

uint8_t databus(void) {
  return PINF;
}

uint8_t databus_write(uint8_t data) {
  PORTF = data;
  return data;
}
