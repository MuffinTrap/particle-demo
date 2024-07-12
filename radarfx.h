#pragma once
#include <mgdl-wii.h>
#include <vector>
#include <string>

class FontGL;

struct NumberEntry
{
	gdl::AlignmentModes alignmentX;
	u32 index;
	guVector lineEnd;
	std::string text;
};

class RadarFX
{
public:
	RadarFX();

	void Init(u32 dotAmount);
	void Update(float deltaTime);
	void Draw(FontGL* font);
	void DrawGrid(float left, float right, float top, float bottom);
	void Quit();

	guVector* dotsArray = nullptr;
	u32 dotAmount = 0;
	u32 gridSize = 16;
	float cellSize;

	// How many points and lines to draw?
	float elapsed = 0.0f;
	u32 drawAmount = 0;

	std::vector<NumberEntry> numberEntries;
};
