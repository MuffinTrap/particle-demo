#include "crossOpenGL.h"
#include "tunerfx.h"
#include "FontGL.h"
#include "palette.h"

TunerFx::TunerFx()
{
	rows = 6;
	step = 0.05f;
	textToRowScale = 0.25f;
	lineSpeed = 0.5f;
	linePos = 0.0f;
}


void TunerFx::Init(int screenWidth, int screenHeight)
{
	aspect = (float)screenWidth/(float)screenHeight;
	width = aspect * 2.0f;
	left = -aspect;
	right = +aspect;
	top = 1.0f;
	bottom = -1.0f;

	names.push_back(CreateName(0.2f, 0, "Beans"));
	names.push_back(CreateName(0.42f, 2, "Muffins"));
	names.push_back(CreateName(0.7f, 2, "and you"));
}

void TunerFx::Update ( float deltaTime )
{
	// Move the orange line
	linePos += lineSpeed * deltaTime;
	if (linePos > width)
	{
		linePos = 0.0f;
	}
}


void TunerFx::Draw ( FontGL* font )
{
	// Draw the border frame

	float z = 0.0f;
	float rowHeight = 2.0f/(float)rows;
	// Draw the guide lines
	float mid = 0.0f;
	float lineWidth = 0.01f;

	glBegin(GL_QUADS);
		PaletteColor3f(ORANGE);
		// Draw the orange line
		glVertex3f(left + linePos, top, z);
		glVertex3f(left + linePos + step/2.0f, top, z);
		glVertex3f(left + linePos + step/2.0f, bottom, z);
		glVertex3f(left + linePos, bottom, z);
	glEnd();

	glBegin(GL_QUADS);
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
			glVertex3f(left, mid+lineWidth, z);
			glVertex3f(right, mid+lineWidth, z);
			glVertex3f(right, mid-lineWidth, z);
			glVertex3f(left, mid-lineWidth, z);

			short steps = (width)/0.05f;
			for (short s = 0; s < steps; s += 1)
			{
				float h = step;
				if (s % 5 == 0)
				{
					h *= 2.0f;
				}
				glVertex3f(left+s*step, mid-h, z);
				glVertex3f(left+s*step+lineWidth, mid-h, z);
				glVertex3f(left+s*step+lineWidth, mid+h, z);
				glVertex3f(left+s*step, mid+h, z);
			}
		}
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
		glm::vec3& p = names[ni].pos;
		if (p.x < left+linePos)
		{
			// Draw cross on the position.
			// Draw in orange if found recently
			float diff = left+linePos - p.x;
			bool close = diff < step;
			ColorName nameColor = WHITE;
			if (close) {
				nameColor = ORANGE;
			}

			glPushMatrix();
			float crossSize = rowHeight * textToRowScale * 0.25f;
			glTranslatef(p.x-crossSize, p.y, p.z);
			// Draw name
			font->Printf(nameColor, rowHeight * textToRowScale, RJustify, Centered, names[ni].text.c_str());

			// Draw cross on position
			glTranslatef(crossSize, -crossSize, 0.0f);
			glBegin(GL_LINES);
				if (close) {
					PaletteColor3f(ORANGE);
				} else{
					PaletteColor3f(GREY);
				}
				glVertex3f(-crossSize, 0.0f, 0.0f);
				glVertex3f(crossSize, 0.0f, 0.0f);

				glVertex3f(0.0f, -crossSize, 0.0f);
				glVertex3f(0.0f, crossSize, 0.0f);
			glEnd();
			glPopMatrix();
		}
	}
}

glm::vec3 TunerFx::GetNamePos(float x, short row)
{
	return glm::vec3(
		left + x * width,
		1.0f - ((float)row +0.5f) * 2.0f/(float)rows,
		0.0f);
}
Name TunerFx::CreateName(float x, short row, const char* text)
{
	Name n;
	n.pos = GetNamePos(x, row);
	n.found = false;
	n.text = text;
	return n;
}
