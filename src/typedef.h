#pragma once

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