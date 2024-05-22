#include "os.h"

#include <stdio.h>
#include <stdlib.h>

#include "TLB.h"
#include "pagetable.h"
#include "memory.h"
#include "storage.h"
#include "utils.h"

// for fifo replacement policy
int mem_head = 0;
int mem_full = 0;
addr_t *ppn_2_vpn;

void os_init()
{
    ppn_2_vpn = (addr_t *)malloc(sizeof(addr_t) * (1 << ppnBits));
    for (int i = 0; i < (1 << ppnBits); i++)
    {
        ppn_2_vpn[i] = -1;
    }
    storage_init();
}

void os_page_missing_handler(addr_t vaddr)
{
    addr_t old_vpn = nullptr;
    addr_t ppn = mem_head;
    addr_t vpn = addr_index(vaddr);
    addr_t offset = addr_offset(vaddr);

    if (mem_full)
    {
        // find the old vpn
        old_vpn = ppn_2_vpn[ppn];

        // update page table for the old page
        pte_t *old_pte = pagetable_get(old_vpn);
        old_pte->valid = 0;
    }

    // load the page from storage
    storage_read(vaddr, memory_get(addr_paddr(ppn, 0)), 1 << offsetBits);
    ppn_2_vpn[ppn] = vpn;

    // update pagetable for the new page
    pte_t *pte = pagetable_get(vpn);
    pte->ppn = ppn;
    pte->valid = 1;

    if (mem_full)
    {
        // update tlb for the new page
        tlb_entry_t *tlb_entry = tlb_get(old_vpn);
        if (tlb_entry!=nullptr)
        {
            tlb_entry->vpn = vpn;
            tlb_entry->ppn = ppn;
        }
    }

    // print
    if (!mem_full)
    {
        printf("    [Load Page] Page %d -> Frame %d\n", vpn, ppn);
    }
    else
    {
        printf("    [Replace page] Frame %d: Page %d -> Page %d\n", ppn, old_vpn, vpn);
    }

    // fifo replacement policy
    mem_head = (mem_head + 1) % (1 << ppnBits);
    if (mem_head == 0)
        mem_full = 1;

    // debug prints
#ifdef VERBOSE
    pagetable_print();
    tlb_print();
    memory_print_page(ppn);
    delay(1000);
#endif
}