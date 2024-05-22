#include "TLB.h"

#include <stdio.h>

#include "pagetable.h"

tlb_entry_t tlb[ENTRIES];

// use FIFO tlb replacement policy
int tlb_head = 0;

void tlb_init()
{
    for (int i = 0; i < ENTRIES; i++)
    {
        tlb[i].valid = 0;
    }
}

tlb_entry_t *tlb_get(addr_t vpn)
{
    for (int i = 0; i < ENTRIES; i++)
        if (tlb[i].valid && tlb[i].vpn == vpn)
            return &tlb[i];

    return nullptr;
}

tlb_entry_t *tlb_replace_fifo(addr_t vpn, addr_t ppn)
{
    tlb_entry_t *entry = &tlb[tlb_head];
    tlb_head = (tlb_head + 1) % ENTRIES;

    entry->vpn = vpn;
    entry->ppn = ppn;
    entry->valid = 1;

    return entry;
}

void tlb_print()
{
    printf("TLB:\n");
    for (int i = 0; i < ENTRIES; i++)
        printf("  %d: (VPN) %d -> (PPN) %d (%c)\n", i, tlb[i].vpn, tlb[i].ppn, tlb[i].valid ? 'V' : ' ');
}