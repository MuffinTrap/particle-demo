#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include "crossTypedefs.h"

#include "FontGL.h"


struct NumberEntry
{
	FontAlignment alignmentX;
	uint32_t index;
	glm::vec3 lineEnd;
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

	glm::vec3* dotsArray = nullptr;
	u32 dotAmount = 0;
	u32 gridSize = 16;
	float cellSize;

	// How many points and lines to draw?
	float elapsed = 0.0f;
	u32 drawAmount = 0;
	unsigned seed = 100;

	std::vector<NumberEntry> numberEntries;
};
