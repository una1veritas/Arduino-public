#include <avr/io.h>

#define bitset(port, bv) (port) |= (bv)
#define bitclear(port, bv) (port) &= ~(bv)

void sram_init(void);
uint8_t sram_read(uint32_t addr);
void sram_write(uint32_t addr, uint8_t data);
void sram_bank(uint8_t bk);

inline void addr_set(uint32_t addr) {
  ADDRL = (uint8_t) addr;
  addr >>= 8;
  ADDRH = (uint8_t) addr;
  addr >>= 8;
  ADDRX &= ~ADDRX_MASK;
  ADDRX |= ((uint8_t)addr) & ADDRX_MASK;
}
