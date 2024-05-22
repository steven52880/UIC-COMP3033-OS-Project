#pragma once

#include "typedef.h"
#include "pagetable.h"

#define ENTRIES 16

typedef struct {
    addr_t vpn;
    addr_t ppn;
    int valid;
} tlb_entry_t;

// extern tlb_entry_t tlb[ENTRIES];

void tlb_init();

tlb_entry_t *tlb_get(addr_t vpn);

tlb_entry_t *tlb_replace_fifo(addr_t vpn, addr_t ppn);

#ifdef DEBUG
void tlb_print();
#endif