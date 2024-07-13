#pragma once
#include <mgdl-wii.h>
#include <string>
#include <vector>

class FontGL;

struct Name
{
	short row;
	short column;
	std::string text;
};

class TunerFx
{
public:
	void Init();
	void Update(float deltaTime);
	void Draw(FontGL* font);

	guVector ColRowPos(float left, short row, short col);

	float linePos = 0.0f;
	short rows = 4;
	float step = 0.05f;

	std::vector<Name> names;
};
