#include "storage.h"
#include <stdlib.h>
#include <stdio.h>

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
    // check if the address is out of bound
    if (addr> storage_size)
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
void storage_print()
{
    // print the first 256 bytes
    const int size = 256;
    uint8_t buf[size];
    fseek(file, 0, SEEK_SET);
    fread(buf, 1, size, file);
    for (int i = 0; i < size; i++)
    {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }    
}
#endif