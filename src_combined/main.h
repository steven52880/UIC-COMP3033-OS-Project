#pragma once
#include <stdio.h>
#include <stdlib.h>

/**
 * typedef.h
*/

typedef unsigned int addr_t;

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef nullptr
#define nullptr 0
#endif

// address space size (2^vaddrBits)
extern addr_t vaddrBits;
// physical memory page number
extern addr_t ppnBits;
// page size (2^offsetBits)
extern addr_t offsetBits;

/**
 * utils.h
 */

addr_t addr_index(addr_t addr);

addr_t addr_offset(addr_t addr);

addr_t addr_addr(addr_t ppn, addr_t offset);

#ifdef DEBUG
void delay(int milliseconds);
#endif

/**
 * statistic.h
*/


extern int statistic_visit_count;
extern int statistic_tlb_hit_count;
extern int statistic_pagefault_count;

void statistic_init();
void count_visit();
void count_tlb_hit();
void count_pagefault();
void statistic_print(FILE *file);

/**
 * storage.h
*/

#define PATH "backingstore.bin"

extern int storage_size;

void storage_init();

int storage_read(addr_t addr, uint8_t *buf, int size);

#ifdef DEBUG
void storage_print(addr_t addr);
#endif

/**
 * memory.h
*/

extern uint8_t *memory;

void memory_init();

addr_t paddr(addr_t ppn, addr_t offset);
uint8_t *memory_get(addr_t paddr);

#ifdef DEBUG
void memory_print_page(addr_t ppn);
#endif

/**
 * pagetable.h
*/

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

/**
 * TLB.h
*/

#define ENTRIES 16

typedef struct
{
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

/**
 * mmu.h
*/

void mmu_init();
uint8_t mmu_get_byte(addr_t vaddr);

/**
 * os.h
*/

extern addr_t *ppn_2_vpn;

void os_init();
void os_page_missing_handler(addr_t vaddr);