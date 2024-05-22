#include "pagetable.h"
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

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