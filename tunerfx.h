#pragma once
#include <string>
#include <vector>
#include "crossVector3.h"

class FontGL;

struct Name
{
	glm::vec3 pos;
	bool found;
	std::string text;
};

class TunerFx
{
public:
	TunerFx();
	void Init(int screenWidth, int screenHeight);
	void Update(float deltaTime);
	void Draw(FontGL* font);


	// Configuration values
	short rows;
	float step;
	float textToRowScale;
	float lineSpeed;

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
