#include "memory.h"

#include <stdlib.h>
#include <stdio.h>

#include "typedef.h"
#include "utils.h"

uint8_t *memory;

void memory_init()
{
    memory = (uint8_t *)malloc(sizeof(uint8_t) * (1 << (ppnBits+offsetBits)));
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