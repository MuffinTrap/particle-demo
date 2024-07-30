#include "rocketdebug.h"

#include <cmath>
#include "crossOpenGL.h"
#include "crossTypedefs.h"
#include "FontGL.h"
#include "palette.h"
#include "platform.h"

#include "src/direction.hpp"

static float* frameTimes;
static size_t index;
static const size_t size = 1024;

void RocketDebug::Init()
{
	frameTimes = (float*)malloc(size * sizeof(float));
	for(size_t i = 0; i < size; i ++)
	{
		frameTimes[i] = 0.0f;
	}
	index = 0;
}

void RocketDebug::Quit()
{
	free(frameTimes);
}


void RocketDebug::Draw ( FontGL* font )
{
	/*  DEBUG THE ROW AND TIMING */
	/*
	glPushMatrix();
	// What is the row
	glTranslatef(-0.9f, 0.0f, -0.5f);
	u32 rowX = static_cast<u32>((get_row()));
	font->Printf(WHITE, 0.1f, LJustify, LJustify, "Row: %.5x", rowX);
	glTranslatef(0.0f, -0.1f, 0.0f);
	font->Printf(WHITE, 0.1f, LJustify, LJustify, "Row F: %f", get_row());
	glTranslatef(0.0f, -0.1f, 0.0f);
	// What is elapsed
	glTranslatef(0.0f, -0.1f, 0.0f);
	font->Printf(WHITE, 0.1f, LJustify, LJustify, "Time: %.2f", getTime());
	glPopMatrix();
	*/

	frameTimes[index] = Platform::GetDeltaTime() / 0.016f;
	index = (index + 1 ) % size;

	float width = 620.0f;
	float height = 64.0f;
	float hh = height/2.0f;

	float top = 480.0f - height - 2;
	float left =(640.0f - width) / 2.0f;


	/* Performance metrics */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640.0f, 480.0f, 0.0f, 1.0f, -1.0f);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	/*
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	font->Printf(GREY, 3.0f, LJustify, LJustify, "Time: %.2f", getTime());
	glPopMatrix();
	*/

	// Box
	glBegin(GL_LINE_STRIP);
	PaletteColor3f(GREY);
		glVertex2f(left, top);
		glVertex2f(left + width, top);
		glVertex2f(left + width, top + height);
		glVertex2f(left, top + height);
		glVertex2f(left, top + hh);
		glVertex2f(left + width, top + hh);
	glEnd();

	// Metrix
	glBegin(GL_LINE_STRIP);
	PaletteColor3f(ORANGE);
	float step = width/(float)size;
	float dx = left + 1.0f;
	for (size_t i = index; i < size; i++)
	{
		glVertex2f(dx, top + height - frameTimes[i] * hh);
		dx += step;
	}
	for (size_t i = 0; i < index; i++)
	{
		glVertex2f(dx, top + height - frameTimes[i] * hh);
		dx += step;
	}
	glEnd();
}
