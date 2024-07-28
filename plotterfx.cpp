#include "plotterfx.h"
#include "crossAlloc.h"
#include "crossOpenGL.h"
#include "palette.h"
#include <math.h>
#include <stdio.h>


void PlotterFX::Init ( int lineAmount )
{
	GeneratePoints(lineAmount, lineAmount);
	plasmaPhase = 0.0f;
}

void PlotterFX::Quit()
{
	if (points != nullptr)
	{
		free(points);
	}
	points = nullptr;
}


void PlotterFX::SetPhase ( float phase )
{
	plasmaPhase = phase;
}

#define YCOLOR(p) \
	if (p.y >= 1.3f) {PaletteColor3f(ORANGE);} \
	else if (p.y >= 0.7f) {PaletteColor3f(WHITE);} \
	else {PaletteColor3f(GREY);} \


void PlotterFX::Draw()
{
	glm::vec3 top = glm::vec3(-1.0f, 2.0f, -1.0f);

	// Draw the box

	// origo is in the center

	float step = 0.25f;


	// Axis lines in orange
	glBegin(GL_LINE_STRIP);
	PaletteColor3f(ORANGE);
		// Axis from 0,0,0
		glVertex3f(-1.0f, 2.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
	glEnd();

	// Y-lines
	glBegin(GL_LINES);
	PaletteColor3f(GREY);
	for (int h = 0; h < 9; h++)
	{
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(top.x+2.0f, top.y, top.z);
		glVertex3f(top.x+2.0f, top.y, top.z);
		glVertex3f(1.0f, top.y, 1.0f);
		top.y -= step;
	}

	// Reset Y
	top.y = 2.0f;
	top.x += step;
	// Z Lines
	for (int h = 0; h < 8; h++)
	{
		// Down
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(top.x, top.y-2.0f, top.z);

		// Depth
		glVertex3f(top.x, top.y-2.0f, top.z);
		glVertex3f(top.x, top.y-2.0f, top.z + 2.0f);
		top.x += step;
	}

	// Reset Y
	top.y = 2.0f;
	top.x = 1.0f;
	// X Lines
	for (int h = 0; h < 8; h++)
	{
		// Down
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(top.x, top.y-2.0f, top.z);

		// Depth
		glVertex3f(top.x, top.y-2.0f, top.z);
		glVertex3f(top.x-2.0f, top.y-2.0f, top.z);
		top.z += step;
	}
	// one more down
	glVertex3f(top.x, top.y, top.z);
	glVertex3f(top.x, top.y-2.0f, top.z);
	glEnd();


	float spread = 3.0f;
	static const float PI =  glm::pi<float>();
	// Calculate plasma
	for (int py = 0; py < depth; py++)
	{
		for(int px = 0; px < width; px++)
		{
			glm::vec3& p = points[py * width + px];

			float lx = px * spread;
			float ly = py * spread;
			float y = (sin(lx*PI/270)*26.0f)+(sin(lx*PI/45)*12.0f)+(sin(((ly+lx)+(8*plasmaPhase))*PI/90)*8);
			y += 0.1f;
			y /= 20.0f;
			p.y=y;
		}

	}

	// Draw the surface
	glBegin(GL_QUADS);
	for (int pz = 0; pz < depth - 1; pz++)
	{
		for (int px = 0; px < width - 1; px++)
		{
			short corner = px + width * pz;
			short next = corner + 1;
			short below = corner + width;
			short across = below + 1;
			glm::vec3& cp = points[corner];
			glm::vec3& np = points[next];
			glm::vec3& ap = points[across];
			glm::vec3& bp = points[below];

			YCOLOR(cp)
			glVertex3f(cp.x, cp.y, cp.z);
			YCOLOR(bp)
			glVertex3f(bp.x, bp.y, bp.z);
			YCOLOR(ap)
			glVertex3f(ap.x, ap.y, ap.z);
			YCOLOR(np)
			glVertex3f(np.x, np.y, np.z);
		}
	}
	glEnd();

	// Epsilon
	float ep = 0.001f;

	// Draw lines in two ways
	for (int py = 0; py < depth; py++)
	{
		glBegin(GL_LINE_STRIP);
		PaletteColor3f(BLACK);
		for(int px = 0; px < width; px++)
		{
			glm::vec3& p = points[py * width + px];
			glVertex3f(p.x, p.y + ep, p.z);
		}
		glEnd();
	}

	for(int px = 0; px < width; px++)
	{
		glBegin(GL_LINE_STRIP);
		PaletteColor3f(BLACK);
		for (int py = 0; py < depth; py++)
		{
			glm::vec3& p = points[py * width + px];
			glVertex3f(p.x, p.y + ep, p.z);
		}
		glEnd();
	}
}

void PlotterFX::GeneratePoints ( int width, int depth )
{
	this->width = width;
	this->depth = depth;
	points = (glm::vec3*)AllocateAlignedMemory(width * depth * sizeof(glm::vec3));

	float hw = (float)width/2.0f;
	float hd = (float)depth/2.0f;
	for (int pz = 0; pz < depth; pz++)
	{
		for(int px = 0; px < width; px++)
		{
			float x = ((float)px-hw)/(float)width;
			float z = ((float)pz-hd)/(float)depth;
			points[pz * width + px] = glm::vec3(x * 2.0f, 0.0f, z * 2.0f);
		}
	}
}



