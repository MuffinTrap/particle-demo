#pragma once
#include <mgdl-wii.h>
#include <GL/opengx.h>
#include "palette.h"
#include "ImageGL.h"
class FontGL : public gdl::FFont
{
public:
	void LoadFromImage(const char* filename, short charw, short charh, char firstCharacter);
	void Printf(ColorName color, float scale, gdl::AlignmentModes alignmentX, gdl::AlignmentModes alignmentY, const char* format, ... );
	void DrawSheet(guVector center);
	void Bind(ImageGL& sheet,  short charw, short charh, char firstCharacter);
private:
	GLuint textureName;
};
