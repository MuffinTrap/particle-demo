#pragma once
#include <mgdl-wii.h>
#include <GL/opengx.h>
#include "palette.h"
class FontGL : public gdl::FFont
{
public:
	void LoadFromImage(const char* filename, short charw, short charh, char firstCharacter);
	void LoadFromBuffer(const void* buffer, size_t size, short charw, short charh, char firstCharacter);
	void Printf(ColorName color, float scale, gdl::AlignmentModes alignmentX, gdl::AlignmentModes alignmentY, const char* format, ... );
	void DrawSheet(guVector center);
private:
	void Bind(gdl::Image& sheet,  short charw, short charh, char firstCharacter);
	GLuint textureName;
};
