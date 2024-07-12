#pragma once
#include <mgdl-wii.h>
#include <GL/opengx.h>
class FontGL : public gdl::FFont
{
public:
	void LoadFromImage(const char* filename, short charw, short charh, char firstCharacter);
	void Printf(float scale, const char* format, ... );
	void DrawSheet(guVector center);
private:
	GLuint textureName;
};
