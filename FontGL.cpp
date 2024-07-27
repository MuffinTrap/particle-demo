#include "FontGL.h"
#include <string.h>
#include <stdarg.h>
#include "crossOpenGL.h"
#include "crossCache.h"
#include "crossTypedefs.h"
#include "crossAssert.h"
#include "crossAlloc.h"

void FontGL::LoadFromImage(const char* filename, short charw, short charh, char firstCharacter )
{
	ImageGL fontImage;
	bool imageOk = fontImage.LoadImage(filename, GL_NEAREST);
	gdl_assert_printf(imageOk, "Did not load font image file: %s", filename);
	textureName = fontImage.textureName; // Store this for rendering the letters
	if (imageOk)
	{
		Bind(fontImage, charw, charh, firstCharacter);
	}

}
void FontGL::Bind ( ImageGL &fontImage, short charw, short charh, char firstCharacter )
{

	printf("Binding font\n");
	short charactersPerRow = fontImage.width/ charw;
	short rows = fontImage.height / charh;
	// Calculate the vertex and texture coordinates (vertices are not used)
	this->firstIndex = firstCharacter;
	this->cw = charw;
	this->ch = charh;
	CreateTextureCoordList(rows, charactersPerRow, fontImage.width, fontImage.height);
	printf("Bind done\n");
}

void FontGL::Printf(ColorName color, float scale, FontAlignment alignmentX, FontAlignment alignmentY, const char* format, ... )
{
	// Draw quads
	va_list args;
	char	buff[256];

	va_start(args, format);
	vsprintf(buff, format, args);
	va_end(args);

	float aspect = (float)cw/(float)ch;
	float step = aspect * scale;

	float dx = 0.0f;
	float dy = 0.0f;
	float dz = 0.0f;

	if (alignmentX == RJustify)
	{
		float width = step * strlen(buff);
		dx -= width;
	}
	else if (alignmentX == Centered)
	{
		float width = step * strlen(buff);
		dx -= width / 2;
	}
	if (alignmentY == RJustify)
	{
		dy += scale;
	}
	else if (alignmentY == Centered)
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
		glm::vec2 tx0 = GetTextureCoordinate(character, 0); // TOP LEFT
		glm::vec2 tx1= GetTextureCoordinate(character, 1); // TOP RIGHT

		glm::vec2 tx2= GetTextureCoordinate(character, 2); // LOW RIGHT
		glm::vec2 tx3= GetTextureCoordinate(character, 3); //LOW LEFT!

		// TOP LEFT
		glTexCoord2f(tx0.x, tx0.y);
		glVertex3f(dx, dy, dz);

		// TOP RIGHT
		glTexCoord2f(tx1.x, tx1.y);
		glVertex3f(dx + step, dy, dz);

		// LOW RIGHT
		glTexCoord2f(tx2.x, tx2.y);
		glVertex3f(dx + step, dy - scale, dz);

		// LOW LEFT!
		glTexCoord2f(tx3.x, tx3.y);
		glVertex3f(dx, dy - scale, dz);

		dx += step;
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
}

void FontGL::DrawSheet ()
{

	float aspect = 128.0f/96.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

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

void FontGL::CreateTextureCoordList(short rows, short charactersPerRow, short texW, short texH)
{
	gdl_assert_print(cw > 0 && ch > 0, "Character dimensions not set");
	gdl_assert_print(rows > 0 && charactersPerRow > 0, "Rows and cpr at zero");
	gdl_assert_print(texW > 0 && texH > 0, "Texture size is 0");

	short characterAmount = rows * charactersPerRow;
	size_t tListSize = (sizeof(glm::vec2)*4)*(characterAmount);
	if (tList == NULL)
	{
		tList = (glm::vec2*)AllocateAlignedMemory(tListSize);
		gdl_assert_print(tList != nullptr, "Out of memory when allocation font txcord list");
	}

	for(short cy=0; cy<rows; cy++) {
		for(short cx=0; cx<charactersPerRow; cx++) {

			// Coordinates to source image
			float tx = cw * cx;
			float ty = ch * cy;
			float tx2 = (tx + cw) -1;
			float ty2 = (ty + ch) -1;

			// Texture coordinate array index
			u32 tc = 4*(cx+(charactersPerRow*cy));

			// Upper-left
			tList[tc].x	= tx/texW;
			tList[tc].y	= ty/texH;

			// Upper-right
			tList[tc+1].x	= tx2/texW;
			tList[tc+1].y	= ty/texH;

			// Lower-right
			tList[tc+2].x	= tx2/texW;
			tList[tc+2].y	= ty2/texH;

			// Lower-left
			tList[tc+3].x	= tx/texW;
			tList[tc+3].y	= ty2/texH;
		}
	}
	CacheFlushRange(tList, tListSize);
}

glm::vec2 FontGL::GetTextureCoordinate(char character, char subIndex)
{
	int	tc = 4*(character - firstIndex);
	return tList[tc + subIndex];
}
