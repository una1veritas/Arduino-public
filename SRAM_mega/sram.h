#include <avr/io.h>

/* SRAM I/F Port/Pin definitions */
#define ADDRL_DIR DDRA
#define ADDRL PORTA
#define ADDRH_DIR DDRC
#define ADDRH PORTC
#define ADDRX_DIR DDRL
#define ADDRX PORTL
#define ADDRX_MASK (1<<0)

#define DATA_OUT  PORTF
#define DATA_IN  PINF
#define DATA_DIR DDRF

#define CONTROL PORTL
#define CONTROL_DIR DDRL
#define SRAM_CS (1<<1)
#define SRAM_OE (1<<2)
#define SRAM_WE (1<<3)
//#define LATCH_L (1<<7)

void sram_init(void);
unsigned char sram_read(unsigned short addr);
void sram_write(unsigned short addr, unsigned char data);

