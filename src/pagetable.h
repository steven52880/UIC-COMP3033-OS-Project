#pragma once

#include "typedef.h"

typedef struct
{
    // 0: on disk, 1: in memory
    int valid;

    addr_t ppn;
} pte_t;

// extern pte_t *pagetable;

void pagetable_init();

pte_t *pagetable_get(addr_t vpn);

#ifdef DEBUG
void pagetable_print();
#endif