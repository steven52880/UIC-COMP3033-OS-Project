#include <stdio.h>
#include <stdlib.h>

#include "mmu.h"
#include "os.h"
#include "statistic.h"
#include "utils.h"

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