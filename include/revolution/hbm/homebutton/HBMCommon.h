#ifndef HBM_COMMON_H
#define HBM_COMMON_H

#include "revolution/mem.h"

extern "C" MEMAllocator* spAllocator;

void* HBMAllocMem(u32 size);
void HBMFreeMem(void* pBlock);

#endif
