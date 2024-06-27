#pragma once
#include <mgdl-wii.h>
#include "particlecloud.h"

class QuadMesh
{
public:

	void* listPtr = nullptr;
	u32 displayListSize = 0;
	gdl::wii::VERT3f32* positionsArray = nullptr;
	RGB8* colorsArray= nullptr;

	void Init(float z);
	void Draw();
};
