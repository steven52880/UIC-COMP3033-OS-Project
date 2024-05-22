#pragma once 
#include "typedef.h"

#include <stdio.h>

extern addr_t *ppn_2_vpn;

void os_init();
void os_page_missing_handler(addr_t vaddr);