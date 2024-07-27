#include "crossOpenGL.h"
#include "crossUtil.h"
#include "tunerfx.h"
#include "FontGL.h"
#include "palette.h"


#include "rocket/track.h"
#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *tuner_pos;  // Position of orange line, 0-1
    const struct sync_track *tuner_names;  // How many names are visible. Change of 1.0f causes a new name to appear on the lines current position.
#else
#include "src/sync_data.h"
#endif

TunerFx::TunerFx()
{
	rows = 6;
	step = 0.05f;
	textToRowScale = 0.25f;
	linePos = 0.0f;
}


void TunerFx::Init(float aspectRatio, sync_device* rocket)
{
	aspect = aspectRatio;
	width = aspect * 2.0f;
	left = -aspect;
	right = +aspect;
	top = 1.0f;
	bottom = -1.0f;

	tuner_pos = sync_get_track(rocket, "tuner_pos");
	tuner_names = sync_get_track(rocket, "tuner_names");

	visibleNames = 0.0f;
	names.push_back(CreateName(0.2f, 0, "Beans"));
	names.push_back(CreateName(0.42f, 2, "Muffins"));
}

void TunerFx::Quit()
{
	save_sync(tuner_pos, "src/sync_data.h");
	save_sync(tuner_names, "src/sync_data.h");

}

void TunerFx::Update ()
{
	linePos = clampF(sync_get_val(tuner_pos, get_row()), 0.0f, 1.0f);
	float newVisible = sync_get_val(tuner_names, get_row());
	float diff =  newVisible - visibleNames;
	while(diff >= 1.0f)
	{
		diff -= 1.0f;
		for (size_t i = 0; i < names.size(); i++)
		{
			if (names[i].found == false)
			{
				// TODO find row that has space on it
				names[i].pos = GetNamePos(linePos, activeNameRow);
				names[i].found = true;
				activeNameRow = (activeNameRow + 1)%rows;
				break;
			}
		}
	}
	if (newVisible > visibleNames)
	{
		visibleNames = newVisible;
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
		if (names[ni].found == false)
		{
			continue;
		}
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

glm::vec3 TunerFx::GetNamePos(float linePosition, short row)
{
	return glm::vec3(
		left + linePosition,
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
