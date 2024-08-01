#include "datacenterfx.h"

#include "crossOpenGL.h"
#include "FontGL.h"

#include "rocket/track.h"
#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *data_floorW;  // How wide the floor is in white tiles x2
    const struct sync_track *data_floorD;  // How deep the floor is in white tiles x2
    const struct sync_track *data_floorLight;  // How white the floor is [0, 1]
    const struct sync_track *data_logoLight;  // How orange the logo on computer is [0, 1]
    const struct sync_track *data_computerLight;  // How white the bottom vertices of the computers are [0,1]
#else
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

void DataCenterFX::Init ( sync_device* rocket )
{
#ifndef SYNC_PLAYER
	data_floorW = sync_get_track(rocket, "data_floorW");
	data_floorD = sync_get_track(rocket, "data_floorD");

	data_floorLight = sync_get_track(rocket, "data:floorLight");
	data_logoLight = sync_get_track(rocket, "data:logoLight");
	data_computerLight = sync_get_track(rocket, "data:computerLight");
#endif
}

void DataCenterFX::Save()
{
#ifndef SYNC_PLAYER
	save_sync(data_floorW, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(data_floorD, SYNC_FILE_H, SYNC_FILE_CPP);

	save_sync(data_floorLight, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(data_logoLight, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(data_computerLight, SYNC_FILE_H, SYNC_FILE_CPP);
#endif
}


void DataCenterFX::Update()
{
	float R = get_row();
	floorWidth = sync_get_val(data_floorW, R);
	floorDepth = sync_get_val(data_floorD, R);

	computerLight = sync_get_val(data_computerLight, R);
	floorLight = sync_get_val(data_floorLight, R);
	logoLight = sync_get_val(data_logoLight, R);
}

void DataCenterFX::Draw(FontGL* font)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	DrawFloor(floorWidth * 2.0f, floorDepth * 2.0f);
	DrawComputers(font);
	// Draw one computer for one width and depth
    glDisable(GL_DEPTH_TEST);
}

void DataCenterFX::Quad(ColorName color, glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
{
	// bottom
	PaletteLerpColor3f(color, computerLight, WHITE);
	glVertex3f(a.x, a.y, a.z);

	// top 1 and 2
	PaletteColor3f(color);
	glVertex3f(b.x, b.y, b.z);
	glVertex3f(c.x, c.y, c.z);

	// bottom2
	PaletteLerpColor3f(color, computerLight, WHITE);
	glVertex3f(d.x, d.y, d.z);
}

// FLOOOR DRAWING

static float size = 1.0f;
static float gap = size / 20.0f;

void DataCenterFX::DrawComputers(FontGL* font)
{
	// Draw computers on intersections : on black tiles

	// starts from origo, on origo is always a black tile
	glPushMatrix();
	short fw = (short)floor(floorWidth);
	short fd = (short)floor(floorDepth);

	// Back left corner
	if (fw == 1)
	{
		glTranslatef(0.0f, 0.0f, 0.0f);
	}
	else
	{
		glTranslatef(-floorWidth/2.0f * (size+gap), 0.0f, 0.0f);
	}
	if (fd == 1)
	{
		glTranslatef(0.0f, 0.0f, 0.0f);
	}
	else
	{
		glTranslatef(0.0f, 0.0f, -floorDepth/2.0f * (size+gap));
	}


	for (short d = 0; d < floorDepth; d++)
	{
		for (short w = 0; w < floorWidth; w++)
		{
			// TODO: push and rotate
			bool rotate = (d > 0 && d%2 == 1);
			if (rotate)
			{
				glPushMatrix();
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			}

			DrawComputer(font, w + d%2);
			if (rotate)
			{
				glPopMatrix();
			}
			glTranslatef((size + gap) * 2.0f, 0.0f, 0.0f);
		}

		// Reset row, advance 2 depths
		glTranslatef((size+gap)* 2.0f *-floorWidth, 0.0f, (size + gap) * 2.0f);
	}
	glPopMatrix();
}

void DataCenterFX::DrawComputer(FontGL* font, short number)
{
	float height = 2.0f;
	glm::vec3 bl = {-0.5f, 0.0f, -0.5f};
	glm::vec3 br = {0.5f, 0.0f, -0.5f};
	glm::vec3 fl = {-0.5f, 0.0f, 0.5f};
	glm::vec3 fr = {0.5f, 0.0f, 0.5f};

	glm::vec3 tbl = {-0.5f, height, -0.5f};
	glm::vec3 tbr = {0.5f, height, -0.5f};
	glm::vec3 tfl = {-0.5f, height, 0.5f};
	glm::vec3 tfr = {0.5f, height, 0.5f};

	glm::vec3 R = {1.0f, 0.0f, 0.0f};
	glm::vec3 U = {0.0f, 1.0f, 0.0f};
	glm::vec3 F = {0.0f, 0.0f, 0.0001f};

	glBegin(GL_QUADS);
		PaletteLerpColor3f(GREY, 0.95f, BLACK);
		// left side
		Quad(BLACK, bl, tbl, tfl, fl);
		// front
		Quad(BLACK, fl, tfl, tfr, fr);
		// right
		Quad(BLACK, fr, tfr, tbr, br);
		// back
		Quad(BLACK, br, tbr, tbl, bl);

		// ID plate
		PaletteColor3f(GREY);
		glm::vec3 id_bl = fl + R * 0.5f + F;
		glm::vec3 id_tl = tfl + R * 0.5f - U * 0.5f + F;
		glm::vec3 id_tr = tfr - R * 0.05f - U * 0.5f + F;
		glm::vec3 id_fr = fr - R * 0.05f + F;
		Quad(GREY, id_bl,
			 id_tl,
			id_tr,
			id_fr);
	glEnd();

	glPushMatrix();
		glTranslatef(id_bl.x + 0.03f, id_bl.y + 0.25f, id_bl.z + 0.0001f);
		font->SetSpacingOnce(-0.05f, 0.0f);
		font->Printf(BLACK, 0.25f, LJustify, RJustify, "%.2X", number);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(tfl.x + 0.1f, tfl.y - 0.1f, tfl.z + 0.001f);
		PaletteLerpColor3f(BLACK, logoLight, ORANGE);
		font->PrintfPreColor(0.3f, LJustify, LJustify, "&");
	glPopMatrix();
}




static float dx = 0;
static float dz = 0;

static void WhiteSquare(float intensity)
{
	float y = 0;
	PaletteLerpColor3f(BLACK, intensity, WHITE);
	glVertex3f(dx-size/2.0f, y, dz-size/2.0f);
	glVertex3f(dx+size/2.0f, y, dz-size/2.0f);
	glVertex3f(dx+size/2.0f, y, dz+size/2.0f);
	glVertex3f(dx-size/2.0f, y, dz+size/2.0f);
}

static void BlackSquare()
{
	float small = size/4.0f + gap * 2.0f;
	float y = 0.001f;
	PaletteColor3f(BLACK);
	glVertex3f(dx, 			y, 		dz-small/2.0f);
	glVertex3f(dx+small/2.0f, y, 	dz);
	glVertex3f(dx, 			y, 		dz+small/2.0f);
	glVertex3f(dx-small/2.0f, y, 	dz);
}

void DataCenterFX::DrawFloor(float wide, float deep)
{
	dx = 0.0f;
	dz = 0.0f;
	//
	//  B B B
	//   W W
	//  B B B
	//   W W
	//	B B B

	// Dimensions in white squares
	short w = (short)floor(wide);
	short d = (short)floor(deep);

	// go to back left corner
	float hw = (float)w/2.0f;
	float hd = (float)d/2.0f;


	glPushMatrix();
	glTranslatef(-hw * (size + gap), 0.0f, -hd * (size + gap));

	glBegin(GL_QUADS);

	for(short di = 0; di < d; di++)
	{
		// Black row
		for(short wi=0; wi <= w; wi++)
		{
			BlackSquare();
			//glTranslatef(size + gap, 0.0f, 0.0f);
			dx += size + gap;
		}

		dx = size/2.0f + gap/2.0f;
		dz += size/2.0f + gap/2.0f;

		// White row
		for(short wi=0; wi < w; wi++)
		{
			WhiteSquare(floorLight);
			dx += size + gap;
		}

		dx = 0.0f;
		dz += size/2.0f + gap/2.0f;
	}

	// Last black row
	for(short wi=0; wi <= w; wi++)
	{
		BlackSquare();
		dx += size + gap;
	}

	glEnd();
	glPopMatrix();
}


