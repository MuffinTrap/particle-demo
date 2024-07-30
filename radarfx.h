#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include "crossTypedefs.h"

#include "rocket/sync.h"
#include "FontGL.h"


struct NumberEntry
{
	FontAlignment alignmentX;
	glm::vec3 lineEnd;
	std::string text;
};

class RadarFX
{
public:
	RadarFX();

	void Init(u32 dotAmount, sync_device* rocket);
	void Update();
	void Draw(FontGL* font);
	void DrawGrid(float left, float right, float top, float bottom);
	void Save();
	void Free();

private:
	void RandomizeDots();

	glm::vec3* dotsArray;
	u32 dotAmount;
	u32 gridSize;
	float cellSize;
	float gap;
	float textHeightRatio;
	float chevronWidthRatio;
	float chevronLengthRatio;
	float symbolWidthRatio;
	float symbolLengthRatio;

	// The rare points are added to number entries
	// in the order they appear.
	// Make sure to add each only once
	u32 raresFound;
	int lastRareFound;
	std::vector<NumberEntry> numberEntries;

	// Rocket controlled:
	u32 drawAmount;
	u32 seed;
	u32 rarity;
	float dotSpread;
	// How many points and lines to draw?
	float elapsed;
};
