#pragma once
#include "typedef.h"

void mmu_init();
uint8_t mmu_get_byte(addr_t vaddr);