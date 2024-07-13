#include <GL/opengx.h>

#include "tunerfx.h"
#include "FontGL.h"
#include "palette.h"

void TunerFx::Init()
{
	names.push_back({0, 4,"Henlo"});
	names.push_back({2, 13, "Muffins!"});
}

void TunerFx::Update ( float deltaTime )
{
	// Move the orange line
	float aspect = (float)gdl::ScreenXres/(float)gdl::ScreenYres;
	linePos += deltaTime;
	if (linePos > aspect * 2.0f)
	{
		linePos = 0.0f;
	}
}

guVector TunerFx::ColRowPos ( float left, short row, short col )
{
	return {
		left + (float)col * step,
		1.0f - ((float)row +0.5f) * 2.0f/(float)rows,
		0.0f};
}

void TunerFx::Draw ( FontGL* font )
{
	// Draw the border frame

	float aspect = (float)gdl::ScreenXres/(float)gdl::ScreenYres;
	float left = -aspect;
	float right = +aspect;
	float top = 1.0f;
	float bottom = -1.0f;
	float z = 0.0f;

	float rowHeight = 2.0f/(float)rows;



	// Draw the guide lines
	float mid = 0.0f;

	glBegin(GL_LINES);
		// Draws
		// |----|----|----|----|
		//
		// |----|----|----|----|
		//
		// |----|----|----|----|
		PaletteColor3f(WHITE);
		for (int h = 1; h < rows; h++)
		{
			mid = top - rowHeight * (float)h;
			glVertex3f(left, mid, z);
			glVertex3f(right, mid, z);

			short steps = (aspect*2.0f)/0.05f;
			for (short s = 0; s < steps; s += 1)
			{
				if (s % 5 == 0)
				{
					glVertex3f(left+s*step, mid+step*2.0f, z);
					glVertex3f(left+s*step, mid-step*2.0f, z);
				}
				else
				{
					glVertex3f(left+s*step, mid+step, z);
					glVertex3f(left+s*step, mid-step, z);
				}
			}
		}
	glEnd();


	glBegin(GL_LINE_LOOP);
		PaletteColor3f(ORANGE);
		// Draw the orange line
		glVertex3f(left + linePos, top, z);
		glVertex3f(left + linePos + step, top, z);
		glVertex3f(left + linePos + step, bottom, z);
		glVertex3f(left + linePos, bottom, z);
	glEnd();

	// Borders
	glBegin(GL_LINE_LOOP);
		PaletteColor3f(WHITE);
		glVertex3f(left, top, z);
		glVertex3f(right, top, z);
		glVertex3f(right, bottom, z);
		glVertex3f(left, bottom, z);
	glEnd();

	// Draw names
	for (size_t ni = 0; ni < names.size(); ni++)
	{
		guVector p = ColRowPos(left, names[ni].row, names[ni].column);
		glPushMatrix();
		glTranslatef(p.x, p.y, p.z);
		font->Printf(WHITE, rowHeight/2.0f, gdl::LJustify, gdl::Centered, names[ni].text.c_str());
		glPopMatrix();
	}
}
