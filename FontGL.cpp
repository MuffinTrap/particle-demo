#include "FontGL.h"
#include <string.h>
#include <mgdl-wii.h>

void FontGL::LoadFromImage(const char* filename, short charw, short charh, char firstCharacter )
{
	ImageGL fontImage;
	bool imageOk = fontImage.LoadImage(filename, gdl::Nearest);
	gdl_assert(imageOk, "Did not load font image file: %s", filename);
	textureName = fontImage.textureName; // Store this for rendering the letters
	Bind(fontImage, charw, charh, firstCharacter);

}
void FontGL::Bind ( ImageGL &fontImage, short charw, short charh, char firstCharacter )
{

	short charactersPerRow = fontImage.width/ charw;
	short rows = fontImage.height / charh;
	// Calculate the vertex and texture coordinates (vertices are not used)
	SetFirstCharacterIndex(firstCharacter);
	SetCharacterDimensions(charw, charh);
	CreateTextureCoordList(rows, charactersPerRow, fontImage.width, fontImage.height);
}

#pragma GCC diagnostic pop

void FontGL::Printf(ColorName color, float scale, gdl::AlignmentModes alignmentX, gdl::AlignmentModes alignmentY, const char* format, ... )
{
	// Draw quads
	va_list args;
	char	buff[256];

	va_start(args, format);
	vsprintf(buff, format, args);
	va_end(args);

	float aspect = (float)GetCharacterWidth()/(float)GetHeight();
	float step = aspect * scale;

	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;

	if (alignmentX == gdl::AlignmentModes::RJustify)
	{
		float width = step * strlen(buff);
		dx -= width;
	}
	else if (alignmentX == gdl::AlignmentModes::Centered)
	{
		float width = step * strlen(buff);
		dx -= width / 2;
	}
	if (alignmentY == gdl::AlignmentModes::RJustify)
	{
		dy += scale;
	}
	else if (alignmentY == gdl::AlignmentModes::Centered)
	{
		dy += scale/2.0f;
	}
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.3f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureName);
    // Discard pixels with low alpha

	glBegin(GL_QUADS);
	PaletteColor3f(color);
	for (short c = 0; buff[c] != '\0'; c++)
	{
		char character = buff[c];
		gdl::wii::TEX2f32 tx0 = GetTextureCoordinate(character, 0); // TOP LEFT
		gdl::wii::TEX2f32 tx1= GetTextureCoordinate(character, 1); // TOP RIGHT

		gdl::wii::TEX2f32 tx2= GetTextureCoordinate(character, 2); // LOW RIGHT
		gdl::wii::TEX2f32 tx3= GetTextureCoordinate(character, 3); //LOW LEFT!

		// TOP LEFT
		glTexCoord2f(tx0.u, tx0.v);
		glVertex3f(dx, dy, dz);

		// TOP RIGHT
		glTexCoord2f(tx1.u, tx1.v);
		glVertex3f(dx + step, dy, dz);

		// LOW RIGHT
		glTexCoord2f(tx2.u, tx2.v);
		glVertex3f(dx + step, dy - scale, dz);

		// LOW LEFT!
		glTexCoord2f(tx3.u, tx3.v);
		glVertex3f(dx, dy - scale, dz);

		dx += step;
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
}

void FontGL::DrawSheet ( guVector center)
{

	float aspect = 128.0f/96.0f;
	float x = center.x;
	float y = center.y;
	float z = center.z;

	glBindTexture(GL_TEXTURE_2D, textureName);
	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);


		// Upper left
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x-aspect*0.5f, y+0.5f, z);

		// Upper right
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x+aspect*0.5f, y+0.5f, z);

		// Lower right
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x+aspect*0.5f, y-0.5f, z);
		glEnd();

		// Lower left
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x-aspect*0.5f, y-0.5f, z);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}
