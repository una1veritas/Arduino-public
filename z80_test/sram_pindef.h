#include <avr/io.h>

/* SRAM I/F Port/Pin definitions */
#define ADDRL_DIR DDRA
#define ADDRL PORTA
#define ADDRL_IN PINA
#define ADDRL_MASK 0xff
#define ADDRH_DIR DDRC
#define ADDRH PORTC
#define ADDRH_IN PINC
#define ADDRH_MASK 0xff
#define ADDRX_DIR DDRL
#define ADDRX PORTL
#define ADDRX_MASK (1<<PL0)

#define DATA_DIR DDRF
#define DATA_OUT PORTF
#define DATA_IN  PINF
#define DATA_MASK 0xff

#define CONTROL_DIR DDRG
#define CONTROL PORTG
#define SRAM_OE (1<<PG1)
#define SRAM_WE (1<<PG0)
//#define SRAM_ALE (1<<PG2)
#define SRAM_CS (1<<PG5)

