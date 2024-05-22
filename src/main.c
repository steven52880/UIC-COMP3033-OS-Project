#include <stdio.h>
#include <stdlib.h>

#include "mmu.h"
#include "os.h"
#include "statistic.h"

#define ADDR_LIST_NAME "addresses.txt"
#define STAT_FILE_NAME "stat.txt"

int main()
{

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