#include "datacenterfx.h"

#include "palette.h"
#include "crossOpenGL.h"
#include "crossVector3.h"
#include "FontGL.h"

void DataCenterFX::Init ( sync_device* rocket )
{

}

void DataCenterFX::Update()
{
}

void DataCenterFX::Draw(FontGL* font)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	DrawFloor();
	DrawComputer(font);
    glDisable(GL_DEPTH_TEST);
}

static void Quad(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
{
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
	glVertex3f(c.x, c.y, c.z);
	glVertex3f(d.x, d.y, d.z);
}

void DataCenterFX::DrawComputer(FontGL* font)
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
	glm::vec3 F = {0.0f, 0.0f, 0.001f};

	glBegin(GL_QUADS);
		PaletteLerpColor3f(GREY, 0.95f, BLACK);
		// left side
		Quad(bl, tbl, tfl, fl);
		// front
		Quad(fl, tfl, tfr, fr);
		// right
		Quad(fr, tfr, tbr, br);
		// back
		Quad(br, tbr, tbl, bl);

		// ID plate
		PaletteColor3f(GREY);
		glm::vec3 id_bl = fl + R * 0.5f + F;
		glm::vec3 id_tl = tfl + R * 0.5f - U * 0.5f + F;
		glm::vec3 id_tr = tfr - U * 0.5f + F;
		glm::vec3 id_fr = fr + F;
		Quad(id_bl,
			 id_tl,
			id_tr,
			id_fr);
	glEnd();

	glPushMatrix();
		glTranslatef(id_bl.x + 0.1f, id_bl.y + 0.1f, id_bl.z + 0.001f);
		font->Printf(BLACK, 0.4f, LJustify, RJustify, "3");
	glPopMatrix();

	glPushMatrix();
		glTranslatef(tfl.x + 0.1f, tfl.y - 0.1f, tfl.z + 0.001f);
		font->Printf(ORANGE, 0.3f, LJustify, LJustify, "&");
	glPopMatrix();
}



// FLOOOR DRAWING

static float size = 1.0f;
static float gap = size / 20.0f;

static float dx = 0;
static float dz = 0;

static void WhiteSquare()
{
	float y = 0;
	PaletteColor3f(WHITE);
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

void DataCenterFX::DrawFloor()
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
	short w = 4;
	short d = 4;

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
			WhiteSquare();
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


