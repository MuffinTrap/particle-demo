#pragma once

#include <cstdio>

extern "C" {

void CacheFlushRange(void* rangeStart, size_t size);

}
