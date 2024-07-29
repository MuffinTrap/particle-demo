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
	for(int i = 0; i < size; i ++)
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

	float width = 640.0f;
	float height = 64.0f;
	float hh = height/2.0f;


	/* Performance metrics */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640.0f, 480.0f, 0.0f, 1.0f, -1.0f);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


	// Box
	glBegin(GL_LINE_STRIP);
	PaletteColor3f(GREY);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(1.0f + width, 1.0f);
		glVertex2f(1.0f + width, 1.0f + height);
		glVertex2f(1.0f, 1.0f + height);
		glVertex2f(1.0f, 1.0f + hh);
		glVertex2f(1.0f + width, 1.0f + hh);
	glEnd();

	// Metrix
	glBegin(GL_LINE_STRIP);
	PaletteColor3f(ORANGE);
	float step = 640.0f/(float)size;
	float dx = 1.0f;
	for (size_t i = index; i < size; i++)
	{
		glVertex2f(dx, 1.0f + height - frameTimes[i] * hh);
		dx += step;
	}
	for (size_t i = 0; i < index; i++)
	{
		glVertex2f(dx, 1.0f + height - frameTimes[i] * hh);
		dx += step;
	}
	glEnd();
}
