#pragma once
#include <mgdl-wii.h>

class RadarFX
{
public:
	RadarFX();

	void Init(u32 dotAmount);
	void Update(float deltaTime);
	void Draw();
	void DrawGrid(float left, float right, float top, float bottom);
	void Quit();

	guVector centerPosition;
	guVector* dotsArray = nullptr;
	u32 dotAmount = 0;
	u32 gridSize = 16;
	float cellSize = 1.0f;

	// How many points and lines to draw?
	float elapsed = 0.0f;
	u32 drawAmount = 0;

};
