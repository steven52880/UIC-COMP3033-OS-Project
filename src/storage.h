#pragma once

#include "typedef.h"

#define PATH "backingstore.bin"

extern int storage_size;

void storage_init();

int storage_read(addr_t addr, uint8_t *buf, int size);

#ifdef DEBUG
void storage_print(addr_t addr);
#endif