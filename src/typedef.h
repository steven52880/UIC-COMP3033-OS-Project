#pragma once
#include <stdint.h>

typedef unsigned int addr_t;
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

// address space size
extern addr_t vaddrBits;
// page size
extern addr_t pageBits;