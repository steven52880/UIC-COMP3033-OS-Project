#include "utils.h"

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