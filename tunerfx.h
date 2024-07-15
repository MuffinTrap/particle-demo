#pragma once
#include <mgdl-wii.h>
#include <string>
#include <vector>

class FontGL;

struct Name
{
	guVector pos;
	bool found = false;
	std::string text;
};

class TunerFx
{
public:
	void Init();
	void Update(float deltaTime);
	void Draw(FontGL* font);


	// Configuration values
	short rows = 6;
	float step = 0.05f;
	float textToRowScale = 0.25f;
	float lineSpeed = 0.5f;

	// Update values
	float linePos = 0.0f;

	std::vector<Name> names;
private:
	guVector GetNamePos(float x, short row);
	Name CreateName(float x, short row, const char* text);

	float aspect;
	float width;
	float left, right, top, bottom;
};
