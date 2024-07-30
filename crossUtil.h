#pragma once

#include "crossTypedefs.h"
// helper functions

extern "C"{


	u32 clampU32(u32 val, u32 minVal, u32 maxVal);
	float clampF(float val, float minVal, float maxVal);
	float minF(float a, float b);
	float maxF(float a, float b);
	u32 minU32(u32 a, u32 b);
	u32 maxU32(u32 a, u32 b);
}
