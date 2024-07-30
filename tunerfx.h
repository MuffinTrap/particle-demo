#pragma once
#include <string>
#include <vector>
#include "crossVector3.h"
#include "rocket/sync.h"

class FontGL;

struct Name
{
	glm::vec3 pos;
	short page;
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
	void Save();


	// Rocket configuration values:
	float linePos = 0.0f;

	// how many names are visible
	float visibleNames = 0.0f;
	// On what row the next name is created
	short activeNameRow = 0;
	// What page of names is shown
	short activeNamePage;

	// How many rows are shown
	short rows;

	// Configuration values
	float step;
	float notchWidth;
	float notchHeight;
	float notchLargeHeight;
	float textToRowScale;
	float lineWidth;


	std::vector<Name> names;
private:
	glm::vec3 GetNamePos(float x, short row);
	Name CreateName(short page, std::string text);

	float aspect;
	float width;
	float left, right, top, bottom;

};
