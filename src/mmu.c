#include "mmu.h"

#include <stdio.h>

#include "memory.h"
#include "pagetable.h"
#include "TLB.h"
#include "utils.h"
#include "os.h"
#include "storage.h"
#include "statistic.h"

void mmu_init()
{
    memory_init();
    pagetable_init();
    tlb_init();
}

uint8_t mmu_get_byte(addr_t vaddr)
{
    if (vaddr >= storage_size)
    {
        printf("Virtual address %d is out of bound\n", vaddr);
        return 0;
    }

#ifdef VERBOSE
    printf("**MMU read address %d\n", vaddr);
#endif

    addr_t vpn = addr_index(vaddr);
    addr_t offset = addr_offset(vaddr);

    // check TLB
    tlb_entry_t *tlb_entry = tlb_get(vpn);
    if (tlb_entry != nullptr)
    {
        // TLB hit
        addr_t paddr = addr_addr(tlb_entry->ppn, offset);
        uint8_t data = *memory_get(paddr);
        printf("[TLB] (LA) %d -> (PA) %d: %d\n", vaddr, paddr, data);
        count_tlb_hit();
        return data;
    }
    
    // TLB miss
    pte_t *pte = pagetable_get(vpn);
    if (pte->valid)
    {
        // data in memory

        // update TLB
        tlb_replace_fifo(vpn, pte->ppn);
        addr_t paddr = addr_addr(pte->ppn, offset);
        uint8_t data = *memory_get(paddr);
        printf("[Page Table] (LA) %d -> (PA) %d: %d\n", vaddr, paddr, data);
        return data;
    }
    
    // memory page missing
    os_page_missing_handler(vaddr);
    count_pagefault();
    return mmu_get_byte(vaddr);
}