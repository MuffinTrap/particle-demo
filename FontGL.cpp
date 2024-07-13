#include "FontGL.h"
#include <string.h>

void FontGL::LoadFromBuffer ( const void* buffer, size_t size, short charw, short charh, char firstCharacter )
{
	gdl::Image fontImage;
	bool imageOk = fontImage.LoadImageBuffer(buffer, size, gdl::Nearest, gdl::RGBA8);
	gdl_assert(imageOk, "Could not load font image from buffer");
	Bind(fontImage, charw, charh, firstCharacter);
}



void FontGL::LoadFromImage(const char* filename, short charw, short charh, char firstCharacter )
{
	gdl::Image fontImage;
	bool imageOk = fontImage.LoadImage(filename, gdl::Nearest, gdl::RGBA8);
	gdl_assert(imageOk, "Did not find font image file: %s", filename);
	Bind(fontImage, charw, charh, firstCharacter);

}
void FontGL::Bind ( gdl::Image& fontImage, short charw, short charh, char firstCharacter )
{

	// Calculate the vertex and texture coordinates (vertices are not used)
	BindSheet(fontImage, charw, charh, firstCharacter);

	short W = fontImage.Texture.TXsize();
	short H = fontImage.Texture.TYsize();
	short glW = W;
	short glH = H;
	GLuint pixels[glH][glW];
	int wx;
	int wy;
	for (int x = 0; x < W; x++)
	{
		for (int y = 0; y < H; y++)
		{
			u32 pixelColor = fontImage.Texture.PeekPixel(x, y);
			wx = x;
			wy = y;
			pixels[wy][wx] = pixelColor;
			/*
			u8 r = (GLubyte)RED(pixelColor);
			u8 g = (GLubyte)GREEN(pixelColor);
			u8 b = (GLubyte)BLUE(pixelColor);
			u8 a = (GLubyte)ALPHA(pixelColor);
			pixels[wy][wx][0]= r; // Red
			pixels[wy][wx][1]= g; // Green
			pixels[wy][wx][2]= b; // Blue
			pixels[wy][wx][3]= a;
			*/
		}
	}

	glGenTextures(1, &textureName);
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
				 glW, glH,
				 0, GL_RGBA, GL_UNSIGNED_BYTE,
			  pixels);

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

	glBindTexture(GL_TEXTURE_2D, textureName);
    // Discard pixels with low alpha
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.3f);

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
	glDisable(GL_ALPHA_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
		// Upper left
		glVertex3f(x-aspect*0.5f, y+0.5f, z);

		// Upper right
		glVertex3f(x+aspect*0.5f, y+0.5f, z);

		// Lower right
		glVertex3f(x+aspect*0.5f, y-0.5f, z);
		glEnd();

		// Lower left
		glVertex3f(x-aspect*0.5f, y-0.5f, z);
	glEnd();

}
