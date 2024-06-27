#pragma once
#include <mgdl-wii.h>

class DeltaHistogram
{
public:
	DeltaHistogram();
	~DeltaHistogram();
	void Update(float value);
	void Draw(int x, int y, int h, u32 color);

	static const int deltasAmount;
	int writeIndex = 0;
	float* deltasPtr;
};
