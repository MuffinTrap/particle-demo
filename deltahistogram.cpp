#include "deltahistogram.h"

const int DeltaHistogram::deltasAmount = 256;

DeltaHistogram::DeltaHistogram()
{
	deltasPtr = (float*)malloc(sizeof(float) * deltasAmount);
	for(u32 i = 0; i < deltasAmount; i++)
	{
		deltasPtr[i] = 0.0f;
	}
	writeIndex = 0;
}

DeltaHistogram::~DeltaHistogram()
{
	free(deltasPtr);
}

void DeltaHistogram::Update(float value)
{
	deltasPtr[writeIndex] = value;
	writeIndex = (writeIndex + 1) % deltasAmount;
}

void DeltaHistogram::Draw ( int x, int y, int h, u32 color )
{
	gdl::DrawBoxF(x,y, x+deltasAmount, y+h, gdl::Color::Blue);
	for(u32 i = 0; i < deltasAmount; i++)
	{
		gdl::DrawLine(x+i, y+h, x+i, y+h-deltasPtr[i]*h, color);
	}
	int i = writeIndex;
	gdl::DrawLine(x+i, y+h, x+i, y+h-deltasPtr[i]*h, gdl::Color::Red);
	gdl::DrawLine(x,y, x+deltasAmount, y, gdl::Color::White);
}
