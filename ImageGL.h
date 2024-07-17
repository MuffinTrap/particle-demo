#pragma once

#include "crossOpenGL.h"

class ImageGL
{
public:
	bool LoadImage(const char* filename, GLuint filterMode);
	GLuint textureName;
	short width;
	short height;

};
