#include "crossAlloc.h"
#include <stdlib.h>

#ifdef GEKKO

void* AllocateAlignedMemory(size_t size) {
	return aligned_alloc(32, size);
}

#else // Win-Mac-Linux

void* AllocateAlignedMemory(size_t size) {
	return malloc(size);
}

#endif
