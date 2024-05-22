#pragma once

#include "typedef.h"

#define PATH "backingstore.bin"

int storage_size;

void storage_init();

int storage_read(addr_t addr, uint8_t *buf, int size);

void storage_print();