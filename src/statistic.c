#include "statistic.h"
#include "stdio.h"
#include "os.h"

int statistic_visit_count;
int statistic_tlb_hit_count;
int statistic_pagefault_count;

void statistic_init()
{
    statistic_visit_count = 0;
    statistic_tlb_hit_count = 0;
    statistic_pagefault_count = 0;
}

void count_visit() {statistic_visit_count++;}

void count_tlb_hit() {statistic_tlb_hit_count++;}

void count_pagefault() {statistic_pagefault_count++;}

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