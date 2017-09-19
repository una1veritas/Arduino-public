#include <inttypes.h>

void z80io_reset();
void init_z80io();
bool z80_busreq();
bool z80_busactivate();
void init_Timer1(uint8_t presc, uint16_t top);

