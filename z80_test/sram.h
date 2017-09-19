#include <inttypes.h>

#define bitset(port, bv) (port) |= (bv)
#define bitclear(port, bv) (port) &= ~(bv)

void sram_init(void);
void sram_bus_release(void);
uint8_t sram_read(uint32_t addr);
void sram_write(uint32_t addr, uint8_t data);
void sram_bank(uint8_t bk);

