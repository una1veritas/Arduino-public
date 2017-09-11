#include <avr/io.h>

/* SRAM I/F Port/Pin definitions */
#define LOW4_DIR DDRC
#define LOW4_OUT PORTC
#define LOW4_IN PINC
#define LOW4_MASK 0x0f
#define HIGH4_DIR DDRD
#define HIGH4_OUT PORTD
#define HIGH4_IN PIND
#define HIGH4_MASK 0xf0
#define LATCH_CONTROL_DIR DDRD
#define LATCH_CONTROL     PORTD
#define LATCH_L           (1<<PD2)
#define LATCH_H           (1<<PD3)

#define SRAM_CONTROL PORTB
#define SRAM_CONTROL_DIR DDRB
//#define SRAM_CS (1<<PD)
#define SRAM_CS 0
#define SRAM_OE (1<<PB0)
#define SRAM_WE (1<<PB1)

void sram_init(void);
unsigned char sram_read(unsigned short addr);
void sram_write(unsigned short addr, unsigned char data);

