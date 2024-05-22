 #pragma once

#include "typedef.h"

extern uint8_t *memory;

void memory_init();

addr_t paddr(addr_t ppn, addr_t offset);
uint8_t *memory_get(addr_t paddr);
void memory_print_page(addr_t ppn);