#pragma once
#include <string>
#include <vector>
#include "crossVector3.h"
#include "rocket/sync.h"

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
	void Init(float aspectRatio, sync_device* rocket);
	void Update();
	void Draw(FontGL* font);
	void Quit();


	// Rocket configuration values:
	float linePos = 0.0f;

	// how many names are visible
	float visibleNames = 0.0f;
	// On what row the next name is created
	short activeNameRow = 0;

	// Configuration values
	short rows;
	float step;
	float textToRowScale;


	std::vector<Name> names;
private:
	glm::vec3 GetNamePos(float x, short row);
	Name CreateName(float x, short row, const char* text);

	float aspect;
	float width;
	float left, right, top, bottom;

};
