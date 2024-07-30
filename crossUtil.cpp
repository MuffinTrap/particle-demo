#include "crossUtil.h"

u32 clampU32(u32 val, u32 minVal, u32 maxVal)
{
	if (val < minVal) return minVal;
	if (val > maxVal) return maxVal;
	return val;
}

float clampF(float val, float minVal, float maxVal)
{
	if (val < minVal) return minVal;
	if (val > maxVal) return maxVal;
	return val;
}

float minF(float a, float b)
{
	return (a < b ? a : b);
}
float maxF(float a, float b)
{
	return (a > b ? a : b);
}
u32 minU32(u32 a, u32 b)
{
	return (a < b ? a : b);
}
u32 maxU32(u32 a, u32 b)
{
	return (a > b ? a : b);
}
