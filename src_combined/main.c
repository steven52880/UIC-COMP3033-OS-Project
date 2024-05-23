#include <stdio.h>
#include <stdlib.h>

#include "main.h"


/**
 * typedef.c
*/

// address space size (2^vaddrBits)
addr_t vaddrBits = 16;
// physical memory page number (2^ppnBits)
addr_t ppnBits = 6;
// page size (2^offsetBits)
addr_t offsetBits = 8;

/**
 * main.c
*/

#define ADDR_LIST_NAME "addresses.txt"
#define STAT_FILE_NAME "stat.txt"

void read()
{
    // read page size and ppn num from input
    while (1)
    {
        printf("Please input frame size (number of bits): ");
        scanf("%d", &offsetBits);
        if (offsetBits < 1)
        {
            printf("Invalid page size\n");
            continue;
        }
        int ppnNum;
        printf("Please input total number of frames: ");
        scanf("%d", &ppnNum);
        if (ppnNum < 1)
        {
            printf("Invalid number of frames\n");
            continue;
        }
        // check if the total size of physical memory (ppnNum) is power of 2
        if ((ppnNum & (ppnNum - 1)) != 0)
        {
            printf("The total size of physical memory must be power of 2\n");
            continue;
        }
        ppnBits = 0;
        while ((1 << ppnBits) < ppnNum)
            ppnBits++;
        // check if the size of physical memory is smaller than logical space
        if (ppnBits + offsetBits >= vaddrBits)
        {
            printf("The size of physical memory must be smaller than logical space\n");
            continue;
        }
        break;
    }
}

int main()
{
    read();

    mmu_init();
    os_init();
    statistic_init();

    FILE *addrlist_fp = fopen(ADDR_LIST_NAME, "r");
    if (addrlist_fp == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", ADDR_LIST_NAME);
        exit(1);
    }
    addr_t addr;
    while (fscanf(addrlist_fp, "%d", &addr) != EOF)
    {
        count_visit();
        mmu_get_byte(addr);
    }
    fclose(addrlist_fp);

    FILE *stat_fp = fopen(STAT_FILE_NAME, "w");
    if (stat_fp == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", STAT_FILE_NAME);
        exit(1);
    }
    statistic_print(stat_fp);
    fclose(stat_fp);

    return 0;
}

/**
 * memory.c
*/

uint8_t *memory;

void memory_init()
{
    memory = (uint8_t *)malloc(sizeof(uint8_t) * (1 << (ppnBits + offsetBits)));
}

uint8_t *memory_get(addr_t paddr)
{
#ifdef VERBOSE
    printf("**Memory get: %d\n", paddr);
#endif
    return &memory[paddr];
}

#ifdef DEBUG
void memory_print_page(addr_t ppn)
{
    addr_t paddr = addr_addr(ppn, 0);
    for (int i = 0; i < (1 << offsetBits); i++)
    {
        printf("%02x ", memory[paddr + i]);
        if ((i + 1) % 8 == 0)
        {
            printf("  ");
        }
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}
#endif

/**
 * mmu.c
*/

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

/**
 * os.c
*/

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
#ifdef VERBOSE
    printf("**Page missing handler: %d\n", vaddr);
#endif
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
    storage_read(addr_addr(vpn, 0), memory_get(addr_addr(ppn, 0)), 1 << offsetBits);
    ppn_2_vpn[ppn] = vpn;

    // update pagetable for the new page
    pte_t *pte = pagetable_get(vpn);
    pte->ppn = ppn;
    pte->valid = 1;

    if (mem_full)
    {
        // update tlb for the new page
        tlb_entry_t *tlb_entry = tlb_get(old_vpn);
        if (tlb_entry != nullptr)
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
    delay(100);
#endif
}

/**
 * pagetable.c
*/

pte_t *pagetable;

void pagetable_init()
{
    pagetable = (pte_t *)malloc(sizeof(pte_t) * (1 << (vaddrBits - offsetBits)));
    for (int i = 0; i < (1 << (vaddrBits - offsetBits)); i++)
    {
        pagetable[i].valid = 0;
        pagetable[i].ppn = 0;
    }
}

pte_t *pagetable_get(addr_t vpn)
{
    if (vpn < 0 || vpn >= (1 << (vaddrBits - offsetBits)))
    {
        return nullptr;
    }
    return &pagetable[vpn];
}

#ifdef DEBUG
void pagetable_print()
{
    printf("Page Table:\n");
    for (int i = 0; i < (1 << (vaddrBits - offsetBits)); i++)
        printf("    vpn: %d -> ppn: %d (%c)\n", i, pagetable[i].ppn, pagetable[i].valid ? 'V' : ' ');
}
#endif

/**
 * storage.c
*/

int storage_size;

FILE *file;
void storage_init()
{
    // open file
    file = fopen(PATH, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", PATH);
        exit(1);
    }

    // get file size
    fseek(file, 0, SEEK_END);
    storage_size = ftell(file);
}

int storage_read(addr_t addr, uint8_t *buf, int size)
{
#ifdef VERBOSE
    printf("**Storage Read: addr=%d, size=%d\n", addr, size);
#endif
    // check if the address is out of bound
    if (addr > storage_size)
    {
        fprintf(stderr, "Storage Error: Address out of bound\n");
        fprintf(stderr, "Address: %d, Size: %d, Storage size: %d\n", addr, size, storage_size);
        exit(1);
    }

    // try to read maximum size
    if (addr + size > storage_size)
        size = storage_size - addr;

    // read from file
    fseek(file, addr, SEEK_SET);
    fread(buf, 1, size, file);

    return size;
}

#ifdef DEBUG
void storage_print(addr_t addr)
{
    const int size = 256;
    uint8_t buf[size];
    storage_read(addr, buf, size);
    for (int i = 0; i < size; i++)
    {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0)
        {
            printf("  ");
        }
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}
#endif

/**
 * statistic.c
*/

int statistic_visit_count;
int statistic_tlb_hit_count;
int statistic_pagefault_count;

void statistic_init()
{
    statistic_visit_count = 0;
    statistic_tlb_hit_count = 0;
    statistic_pagefault_count = 0;
}

void count_visit() { statistic_visit_count++; }

void count_tlb_hit() { statistic_tlb_hit_count++; }

void count_pagefault() { statistic_pagefault_count++; }

void statistic_print(FILE *file)
{
#ifdef DEBUG
    printf("Visit count: %d\n", statistic_visit_count);
    printf("TLB hit count: %d\n", statistic_tlb_hit_count);
    printf("Page fault count: %d\n", statistic_pagefault_count);
#endif
    fprintf(file, "TLB hit rate: %.3f\n\n", (double)statistic_tlb_hit_count / statistic_visit_count);

    fprintf(file, "Page-fault rate: %.3f\n\n", (double)statistic_pagefault_count / statistic_visit_count);

    fprintf(file, "Memory image:\n");

    for (int i = 0; i < (1 << ppnBits); i += 16)
    {
        fprintf(file, "Frame %d ~ Frame %d:", i, i + 15);
        for (int j = 0; j < 16; j++)
        {
            if (i + j < (1 << ppnBits))
                fprintf(file, " %d", ppn_2_vpn[i + j]);
        }
        fprintf(file, "\n");
    }
}

/**
 * tlb.c
*/

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

#ifdef DEBUG
void tlb_print()
{
    printf("TLB:\n");
    for (int i = 0; i < ENTRIES; i++)
        printf("  %d: (VPN) %d -> (PPN) %d (%c)\n", i, tlb[i].vpn, tlb[i].ppn, tlb[i].valid ? 'V' : ' ');
}
#endif

/**
 * utils.c
*/

// generate a 32-bit integer with n trailing 1s
addr_t bits_n1(int n)
{
    return (1 << n) - 1;
}

/*
vaddr (16bit): [index, offset]
*/

addr_t addr_index(addr_t addr)
{
    return addr >> offsetBits;
}

addr_t addr_offset(addr_t addr)
{
    return addr & bits_n1(offsetBits);
}

addr_t addr_addr(addr_t ppn, addr_t offset)
{
    return (ppn << offsetBits) + offset;
}

#ifdef DEBUG
#include <time.h>
void delay(int milliseconds)
{
    clock_t start = clock();
    while ((clock() - start) * 1000 / CLOCKS_PER_SEC < milliseconds)
        ;
}
#endif