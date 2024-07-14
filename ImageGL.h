#pragma once

#include <GL/opengx.h>
#include <mgdl-wii.h>

class ImageGL
{
public:
	bool LoadImage(const char* filename, gdl::TextureFilterModes filter);
	GLuint textureName;
	short width;
	short height;

};
