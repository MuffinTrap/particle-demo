#include "ImageGL.h"
#include <stdio.h>
#include <png.h>
#include <cstdlib>
#include "crossCache.h"
#include "crossAlloc.h"

struct TextureGL
{
	GLsizei width;
	GLsizei height;
	GLenum format;
	GLint bytesPerPixel;
	GLubyte *texels;
};

static bool GetPNGtextureInfo(int color_type, TextureGL* texInfo)
{
	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			texInfo->format = GL_LUMINANCE;
			texInfo->bytesPerPixel = 1;
		break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			texInfo->format = GL_LUMINANCE_ALPHA;
			texInfo->bytesPerPixel = 2;
		break;
		case PNG_COLOR_TYPE_RGB:
			texInfo->format = GL_RGB;
			texInfo->bytesPerPixel = 3;
		break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			texInfo->format = GL_RGBA;
			texInfo->bytesPerPixel = 4;
		break;
		default:
			printf("Unsupported PNG COLOR TYPE!\n");
			return false;
		break;
	};
	return true;
}

static TextureGL* ReadPNG(const char* filename)
{
	FILE *fp = fopen(filename, "rb");
	if (fp == nullptr)
	{
		printf("[NOT FOUND!]\n");
		fclose(fp);
		return nullptr;
	}

	TextureGL* textureInfoPtr = nullptr;
	png_byte magic[8];
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers = nullptr;
	int bit_depth, color_type;

	// Read Magic number :O
	fread(magic, 1, sizeof(magic), fp);
	if (png_check_sig(magic, sizeof(magic)) == false)
	{
		printf("[INVALID PNG]\n");
		fclose(fp);
		return nullptr;
	}

	// Create struct for reading the file
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == nullptr)
	{
		printf("[FAILED TO READ]\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		return nullptr;
	}

	// Create struct to hold info about the file
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == nullptr)
	{
		printf("[FAILED TO GET INFO]\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		return nullptr;
	}

	textureInfoPtr = new TextureGL();

	// This sets what happens if there is an error during read
	if (setjmp (png_jmpbuf(png_ptr)))
	{
		if (row_pointers != nullptr)
		{
			free(row_pointers);
		}
		if (textureInfoPtr != nullptr)
		{
			if (textureInfoPtr->texels != nullptr)
			{
				free (textureInfoPtr->texels);
			}
			delete textureInfoPtr;
		}
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return nullptr;
	}

	printf("init io...");
	// Lets read the file now!
	png_init_io(png_ptr, fp);

	// Skip magic number since we read it already
	png_set_sig_bytes(png_ptr, sizeof(magic));

	printf("Read image info: ");
	png_read_info(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);

	// React to info we got
	printf("Color type: ");
	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		// Read this as RGB image
		printf("palette ");
		png_set_palette_to_rgb(png_ptr);
	}
	else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		// Small precision grayscale to 8 bit format
		printf("grayscale < 8 ");
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	else
	{
		printf("RGB ");
	}
	printf("Alpha: ");
	// Is transparency info read ok?
	png_uint_32 getResult = png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
	if (getResult == 0)
	{
		printf("YES ");
		// Direct it to alpha channel
		png_set_tRNS_to_alpha(png_ptr);
	}
	else
	{
		printf("NO ");
	}

	printf("Bit depth: ");
	if (bit_depth == 16)
	{
		printf("16 ");
		png_set_strip_16(png_ptr);
	}
	else if (bit_depth < 8)
	{
		// ???
		printf("<8 ");
		png_set_packing(png_ptr);
	}
	else
	{
		printf("%d ", bit_depth);
	}

	// Update our changes and read again
	png_read_update_info(png_ptr, info_ptr);
	png_uint_32 w, h;
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, NULL, NULL, NULL);

	// Wii cannot handle very big textures
	if ((w > 1024) || (h > 1024)) {
		printf("[TOO LARGE: MAX 1024]\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		delete textureInfoPtr;
		return nullptr;
	}
	else
	{
		printf("Size: %u x %u\n", w, h);
		textureInfoPtr->width = w;
		textureInfoPtr->height = h;
	}

	bool colorOk = GetPNGtextureInfo(color_type, textureInfoPtr);
	if (colorOk == false)
	{
		printf("[STRANGE PNG COLOR TYPE]\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		delete textureInfoPtr;
		return nullptr;
	}

	// Allocate!
	size_t imageDataSize = sizeof(GLubyte) * w * h * textureInfoPtr->bytesPerPixel;
	printf("Allocate %lu bytes\n", imageDataSize);
	textureInfoPtr->texels = (GLubyte*)AllocateAlignedMemory(imageDataSize);

	if (textureInfoPtr->texels == nullptr)
	{
		printf("[OUT OF MEMORY]\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		delete textureInfoPtr;
		return nullptr;
	}

	// Set up array for rows to read
	size_t rowPointersSize = sizeof(png_bytep) * h;
	row_pointers = (png_bytep*)AllocateAlignedMemory(rowPointersSize);

	// Read all rows
	size_t rowSize = w * textureInfoPtr->bytesPerPixel;

	// Is the first row at the start or at the end?
// #define READ_BOTTOM_UP

	for (png_uint_32 i = 0; i < h; i++)
	{
#ifdef READ_BOTTOM_UP
		size_t readStart = (h - (i+1)) * rowSize;
#else
		size_t readStart = i * rowSize;
#endif
		row_pointers[i] = (png_bytep)(textureInfoPtr->texels + readStart);
	}
	CacheFlushRange(row_pointers, rowPointersSize);

	// Pointers to rows are set, read the pixels on the rows
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	// Reading is over, rows no longer needed
	free(row_pointers);
	fclose(fp);

	printf("PNG read done\n");

	// Flush read texels
	CacheFlushRange(textureInfoPtr->texels, imageDataSize);
	return textureInfoPtr;
}

bool ImageGL::LoadImage ( const char* filename, GLuint filterMode)
{
	// Load using png
	printf("gdl:: Loading image %s for OpenGX...", filename);

	TextureGL* textureDataPtr = ReadPNG(filename);
	if (textureDataPtr == nullptr || textureDataPtr->texels == nullptr)
	{
		return false;
	}
	width = textureDataPtr->width;
	height = textureDataPtr->height;

	GLint alignment;
	glGenTextures(1, &textureName);
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

	// Load to OpenGL

	// Read current alignment and set alignment to 1
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// We dont have mipmaps..
	glTexImage2D(GL_TEXTURE_2D, 0, textureDataPtr->bytesPerPixel,
			  textureDataPtr->width, textureDataPtr->height,
			  0, textureDataPtr->format,
			  GL_UNSIGNED_BYTE,
			  textureDataPtr->texels);

	// restore previous alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

	// Data is loaded to OpenGX, release the buffers
	free(textureDataPtr->texels);
	free(textureDataPtr);
	return true;
}
