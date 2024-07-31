#pragma once

#include "crossOpenGL.h"
struct TextureGL;

class ImageGL
{
public:
	bool LoadImageBuffer(const void* buffer, size_t size, GLuint filterMode);
	bool LoadImage(const char* filename, GLuint filterMode);
	void Draw2D(float cx, float cy, float scale, bool alphaEnabled);
	GLuint textureName;
	short width;
	short height;
private:
	bool Load ( TextureGL* textureDataPtr, GLuint filterMode );
};
