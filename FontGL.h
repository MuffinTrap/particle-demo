#pragma once
#include "palette.h"
#include "ImageGL.h"
#include "crossVector3.h"

enum FontAlignment
{
	Centered,
	RJustify,
	LJustify
};

class FontGL
{
public:
	void LoadFromImage(const char* filename, short charw, short charh, char firstCharacter);
	void Printf(ColorName color, float scale, FontAlignment alignmentX, FontAlignment alignmentY, const char* format, ... );
	void DrawSheet();
	void Bind(ImageGL& sheet,  short charw, short charh, char firstCharacter);
	void SetSpacingOnce(float x, float y);
private:

	float spacingX;
	float spacingY;

	void CreateTextureCoordList(short rows, short charactersPerRow, short texW, short texH);
	glm::vec2 GetTextureCoordinate(char character, char subIndex);
	GLuint textureName;
	short		cw,ch;
	glm::vec2		*tList;
	char		firstIndex; // muffintrap: need to remember the first index to calculate offsets into vertex and uv arrays
};
