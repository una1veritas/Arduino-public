#include <avr/io.h>

/* SRAM I/F Port/Pin definitions */
#define ADDRL_DIR DDRA
#define ADDRL PORTA
#define ADDRL_MASK 0xff
#define ADDRH_DIR DDRC
#define ADDRH PORTC
#define ADDRH_MASK 0xff
#define ADDRX_DIR DDRD
#define ADDRX PORTD
#define ADDRX_MASK (1<<PD7)

#define DATA_DIR DDRA
#define DATA_OUT  PORTA
#define DATA_IN  PINA

#define CONTROL_DIR DDRG
#define CONTROL PORTG
#define SRAM_OE (1<<PG1)
#define SRAM_WE (1<<PG0)
#define SRAM_ALE (1<<PG2)
#define CONTROL_CS_DIR DDRL
#define CONTROL_CS PORTL
#define SRAM_CS (1<<PL6)

#define bitset(port, bv) (port) |= (bv)
#define bitclear(port, bv) (port) &= ~(bv)

void sram_init(void);
uint8_t sram_read(uint16_t addr);
void sram_write(uint16_t addr, uint8_t data);
void sram_bank(uint8_t bk);

