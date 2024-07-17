#pragma once
#include <string>
#include <vector>
#include "crossVector3.h"

class FontGL;

struct Name
{
	glm::vec3 pos;
	bool found = false;
	std::string text;
};

class TunerFx
{
public:
	void Init(int screenWidth, int screenHeight);
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
	glm::vec3 GetNamePos(float x, short row);
	Name CreateName(float x, short row, const char* text);

	float aspect;
	float width;
	float left, right, top, bottom;
};
