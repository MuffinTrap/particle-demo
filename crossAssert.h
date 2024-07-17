#pragma once

#ifdef GEKKO
#include <mgdl-wii.h>
#include <mgdl-wii/mgdl-assert.h>

#else
#include <cassert>

extern "C"{

void AssertFunctionPrintf(bool test, const char* filename, int lineNumber, const char* message, ...);

void AssertFunctionPrint(bool test, const char* filename, int lineNumber, const char* message);

}

#define gdl_assert_printf(test, message, ...) \
AssertFunctionPrintf(test, __FILE__, __LINE__, message, __VA_ARGS__)

#define gdl_assert_print(test, message) \
AssertFunctionPrint(test, __FILE__, __LINE__, message)

#define gdl_assert_test(test) assert(test)

#endif
