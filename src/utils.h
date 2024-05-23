#pragma once
#include "typedef.h"

addr_t addr_index(addr_t addr);

addr_t addr_offset(addr_t addr);

addr_t addr_addr(addr_t ppn, addr_t offset);

#ifdef DEBUG
void delay(int milliseconds);
#endif