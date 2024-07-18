#include "crossCache.h"

#ifdef GEKKO
#include <mgdl-wii.h>

void CacheFlushRange ( void* rangeStart, size_t size )
{
	DCFlushRange(rangeStart, size);
}

#else

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void CacheFlushRange ( void* rangeStart, size_t size )
{
	// nop
}
#pragma GCC diagnostic pop
#endif
