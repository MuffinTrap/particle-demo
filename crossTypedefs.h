#pragma once


// Export ogc type names to other platforms

#ifdef GEKKO

#include <gctypes.h>

#else

#include <cstdint>
typedef uint32_t u32;

#endif
