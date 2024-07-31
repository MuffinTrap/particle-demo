#include "plotterfx.h"
#include "crossAlloc.h"
#include "crossOpenGL.h"
#include "crossTypedefs.h"
#include "crossUtil.h"
#include "palette.h"

#include <math.h>
#include <stdio.h>

#include "rocket/track.h"
#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *plotter_impact;  // How much impact a particle has
    const struct sync_track *plotter_decay;  // how fast the heat decays
    const struct sync_track *plotter_updateHz;  // how many frames to skip between updates
#else
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

static const float AREA_SZ = 2.0f;
static const float AREA_H = 2.0f;
static const float MIN_AXIS = -1.0f;
static const float MAX_AXIS = 1.0f;

static const float ydrop = -0.5f;



void PlotterFX::Init ( int lineAmount, sync_device* rocket )
{
	GeneratePoints(lineAmount, lineAmount);
	plasmaPhase = 0.0f;
#ifndef SYNC_PLAYER
	plotter_impact = sync_get_track(rocket, "plotter_impact");
	plotter_decay = sync_get_track(rocket, "plotter_decay");
	plotter_updateHz = sync_get_track(rocket, "plotter_updateHz");
#endif

	frameCounter= 0;
}

void PlotterFX::Free()
{
	if (points != nullptr)
	{
		free(points);
	}
	points = nullptr;
}

void PlotterFX::Save()
{
#ifndef SYNC_PLAYER
	save_sync(plotter_impact, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(plotter_decay, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(plotter_updateHz, SYNC_FILE_H, SYNC_FILE_CPP);
#endif
}


void PlotterFX::SetPhase ( float phase )
{
	plasmaPhase = phase;
}

void PlotterFX::Update(Particle* particlesArray, u32 amount)
{
	u32 interval = maxU32(1, (u32)sync_get_val(plotter_updateHz, get_row()));
	frameCounter++;
	if (frameCounter % interval == 0)
	{
		frameCounter = 0;
		ReadParticles(particlesArray, amount);
	}
}

static ColorName color_a;
static ColorName color_b;
static float color_t;

#define YCOLOR(py) \
	if (py > 0.5f) { color_t = (py-0.5f)/0.5f; color_a = WHITE; color_b = ORANGE; } \
	else {color_t = py/0.5f; color_a = GREY; color_b = WHITE;} \
	PaletteLerpColor3f(color_a, color_t, color_b);


void PlotterFX::DrawBox()
{
	const float ceilingY = MAX_AXIS;
	glm::vec3 top = glm::vec3(MIN_AXIS, ceilingY, MIN_AXIS);

	// Draw the box

	// origo is in the center

	float step = 0.25f;


	// Axis lines in orange
	glBegin(GL_LINE_STRIP);
	PaletteColor3f(ORANGE);
		// Axis from 0,0,0
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(MIN_AXIS, MIN_AXIS, MIN_AXIS);
		glVertex3f(MIN_AXIS, MIN_AXIS, MAX_AXIS);
		glVertex3f(MAX_AXIS, MIN_AXIS, MAX_AXIS);
	glEnd();

	// Y-lines
	glBegin(GL_LINES);
	PaletteColor3f(GREY);
	for (int h = 0; h < 9; h++)
	{
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(top.x+AREA_SZ, top.y, top.z);
		glVertex3f(top.x+AREA_SZ, top.y, top.z);
		glVertex3f(MAX_AXIS, top.y, MAX_AXIS);
		top.y -= step;
	}

	// Reset Y
	top.y = ceilingY;
	top.x += step;
	// Z Lines
	for (int h = 0; h < 8; h++)
	{
		// Down
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(top.x, top.y-AREA_H, top.z);

		// Depth
		glVertex3f(top.x, top.y-AREA_H, top.z);
		glVertex3f(top.x, top.y-AREA_H, top.z + AREA_SZ);
		top.x += step;
	}

	// Reset Y
	top.y = ceilingY;
	top.x = MAX_AXIS;
	// X Lines
	for (int h = 0; h < 8; h++)
	{
		// Down
		glVertex3f(top.x, top.y, top.z);
		glVertex3f(top.x, top.y-AREA_H, top.z);

		// Depth
		glVertex3f(top.x, top.y-AREA_H, top.z);
		glVertex3f(top.x-AREA_SZ, top.y-AREA_H, top.z);
		top.z += step;
	}
	// one more down
	glVertex3f(top.x, top.y, top.z);
	glVertex3f(top.x, top.y-AREA_H, top.z);
	glEnd();

}

void PlotterFX::DrawLines(ColorName colo, int skip)
{
	if (skip < 1)
	{
		skip = 1;
	}
	// Epsilon
	float ep = 0.001f;

	PaletteColor3f(colo);
	// Draw lines in two ways
	for (u32 py = 0; py < depth; py += skip)
	{
		glBegin(GL_LINE_STRIP);
		for(u32 px = 0; px < width; px += skip)
		{
			glm::vec4& p = points[py * width + px];
			glVertex3f(p.x, p.y + ep, p.z);
		}
		glEnd();
	}
	// always draw last

	for(u32 px = 0; px < width; px += skip)
	{
		glBegin(GL_LINE_STRIP);
		for (u32 py = 0; py < depth; py += skip)
		{
			glm::vec4& p = points[py * width + px];
			glVertex3f(p.x, p.y + ep, p.z);
		}
		glEnd();
	}
}

void PlotterFX::DrawSurface()
{
	// Draw the surface
	glBegin(GL_QUADS);
	for (u32 pz = 0; pz < depth - 1; pz++)
	{
		for (u32 px = 0; px < width - 1; px++)
		{
			short corner = px + width * pz;
			short next = corner + 1;
			short below = corner + width;
			short across = below + 1;
			glm::vec4& cp = points[corner];
			glm::vec4& np = points[next];
			glm::vec4& ap = points[across];
			glm::vec4& bp = points[below];

			YCOLOR(fabs(cp.y))
			glVertex3f(cp.x, cp.y, cp.z);
			YCOLOR(fabs(bp.y))
			glVertex3f(bp.x, bp.y, bp.z);
			YCOLOR(fabs(ap.y))
			glVertex3f(ap.x, ap.y, ap.z);
			YCOLOR(fabs(np.y))
			glVertex3f(np.x, np.y, np.z);
		}
	}
	glEnd();
}


void PlotterFX::Draw()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	DrawBox();
	glPushMatrix();
	glTranslatef(0.0f, ydrop, 0.0f);
	DrawSurface();
	//DrawLines(BLACK, 3);
	glPopMatrix();

    glDisable(GL_DEPTH_TEST);
}

void PlotterFX::GeneratePoints ( int w, int d)
{
	this->width = w;
	this->depth = d;
	points = (glm::vec4*)AllocateAlignedMemory(width * depth * sizeof(glm::vec4));

	float fd = (float)depth;
	float fw = (float)width;
	float hw = fw/2.0f;
	float hd = fd/2.0f;
	for (u32 pz = 0; pz < depth; pz++)
	{
		float z = ((float)pz-hd)/fd;
		if (pz == depth-1)
		{
			// At the very end. pz never reaches depth
			z = (fd-hd)/fd;
		}
		for(u32 px = 0; px < width; px++)
		{
			float x = ((float)px-hw)/fw;
			if (px == width-1)
			{
				x = (fw-hw)/fw;
			}

			// x and z are in [-0.5, 0.5f] range
			points[pz * width + px] = glm::vec4(x * AREA_SZ, 0.0f, z * AREA_SZ, 0.0f);
		}
	}
}

void PlotterFX::ReadParticles(Particle* particles, u32 particle_amount)
{

	const float decay = sync_get_val(plotter_decay, get_row());
	const float impact = sync_get_val(plotter_impact, get_row());
	const float maxHits = 1.0f/impact;
	const float cellsize = AREA_SZ / (float)width;
	for (u32 i = 0; i < width * depth; i++)
	{
		// decrease heat
		points[i].y *= decay;
		points[i].w = 1.0f;
	}

	for (u32 i = 0; i < particle_amount; i ++)
	{
		// map to [0, AREA_SZ]
		float x = (particles[i].position.x) + 2.0f;
		float z = (particles[i].position.z) + 2.0f;

		x *= 0.5f;
		z *= 0.5f;

		// Cell: [0, 63]
		float gridx = ((x) / cellsize);
		float gridz = ((z) / cellsize);

		// Clamp to be inside points array
		// [0, width-1] x [0, depth-1]
		u32 px = (u32)floor(gridx);
		u32 pz = (u32)floor(gridz);

		if (px >= 0 && px < width && pz >= 0 && pz < depth)
		{
			u32 pi = px + pz * width;

			// Calculate heatmap: w is the count of samples on this point
			points[pi].y += impact;
			points[pi].w += 1.0f;
		}
	}

	for (u32 i = 0; i < width * depth; i++)
	{
		const float w= points[i].w;
		const float exp = maxF(w/maxHits, 1.0f);
		points[i].y *= exp * exp;
	}
}


void PlotterFX::ApplyPlasma()
{
	float spread = 3.0f;
	static const float glmPI =  glm::pi<float>();
	// Calculate plasma
	for (u32 py = 0; py < depth; py++)
	{
		for(u32 px = 0; px < width; px++)
		{
			glm::vec4& p = points[py * width + px];

			float lx = px * spread;
			float ly = py * spread;
			float y = (sin(lx*glmPI/270)*26.0f)+(sin(lx*glmPI/45)*12.0f)+(sin(((ly+lx)+(8*plasmaPhase))*glmPI/90)*8);
			y += 0.1f;
			y /= 20.0f;
			p.y=y;
		}
	}
}


