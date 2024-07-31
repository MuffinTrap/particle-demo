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
    const struct sync_track *tuner_row;  // On what row to place the next name. 0 is top row
    const struct sync_track *tuner_page;  // From what page the next name is taken
#else
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

TunerFx::TunerFx()
{
	rows = 6;
	textToRowScale = 0.25f;
	linePos = 0.0f;
	notchWidth = 0.01f;
	notchHeight = 0.03f;
	notchLargeHeight =  0.06f;
	lineWidth = 0.01f;
}


void TunerFx::Init(float aspectRatio, sync_device* rocket)
{
	aspect = aspectRatio;
	width = aspect * 2.0f;
	left = -aspect;
	right = +aspect;
	top = 1.0f;
	bottom = -1.0f;

#ifndef SYNC_PLAYER
	tuner_pos = sync_get_track(rocket, "tuner_pos");
	tuner_names = sync_get_track(rocket, "tuner_names");
	tuner_row = sync_get_track(rocket, "tuner_row");
	tuner_page = sync_get_track(rocket, "tuner_page");
#endif

	visibleNames = 0.0f;
	short page = 0;
	names.push_back(CreateName(page, "FURRY"));
	names.push_back(CreateName(page, "TRASH"));
	names.push_back(CreateName(page, "GROUP"));

	names.push_back(CreateName(page, "JML"));
	names.push_back(CreateName(page, "TUHB"));
	names.push_back(CreateName(page, "RIFT"));

	page += 1;
	names.push_back(CreateName(page, "TPOLM"));
	names.push_back(CreateName(page, "SLIPSTREAM"));
	names.push_back(CreateName(page, "SVATG"));

	names.push_back(CreateName(page, "TRUCK"));
	names.push_back(CreateName(page, "EPOCH"));
	names.push_back(CreateName(page, "QUADTRIP"));

	page += 1;
	names.push_back(CreateName(page, "PEISIK"));
	names.push_back(CreateName(page, "FARMIYAH"));
	names.push_back(CreateName(page, "IVORY"));

	names.push_back(CreateName(page, "MFX"));
	names.push_back(CreateName(page, "KEWLERS"));
	names.push_back(CreateName(page, "MERCURY"));

	page += 1;
	names.push_back(CreateName(page, "BASSCADET"));
	names.push_back(CreateName(page, "GARGAJ"));
	names.push_back(CreateName(page, "BUS ERROR"));
	names.push_back(CreateName(page, "LAMEGUY64"));

	page += 1;
	names.push_back(CreateName(page, "RYOJI IKEDA"));
	names.push_back(CreateName(page, "DEVKITPRO"));
	names.push_back(CreateName(page, "NINTENDO R&D"));
}

void TunerFx::Save()
{
#ifndef SYNC_PLAYER
	save_sync(tuner_pos, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(tuner_names, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(tuner_row, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(tuner_page, SYNC_FILE_H, SYNC_FILE_CPP);
#endif
}

void TunerFx::Update ()
{
	float R = get_row();
	float prevpos = linePos;
	linePos = width * sync_get_val(tuner_pos, R);
	float newVisible = sync_get_val(tuner_names, R);
	activeNameRow = sync_get_val(tuner_row, R);
	activeNamePage = sync_get_val(tuner_page, R);
	float diff =  newVisible - visibleNames;
	while(diff >= 1.0f)
	{
		diff -= 1.0f;
		for (size_t i = 0; i < names.size(); i++)
		{
			if (names[i].page == activeNamePage && names[i].found == false)
			{
				names[i].pos = GetNamePos(linePos, activeNameRow);
				names[i].found = true;
				break;
			}
		}
	}
	if (newVisible > visibleNames)
	{
		visibleNames = newVisible;
	}

	// if linepos moves backward, hide all found names on this page and decrease counter
	if (prevpos > linePos) {
		for (size_t i = 0; i < names.size(); i++)
		{
			if (names[i].page == activeNamePage && names[i].found == true)
			{
				names[i].found = false;
				visibleNames -= 1;
			}
		}
	}
}


void TunerFx::Draw ( FontGL* font )
{
	// Draw the border frame

	float z = 0.0f;
	float rowHeight = 2.0f/(float)rows;
	// Draw the guide lines
	float mid = 0.0f;

	glBegin(GL_QUADS);
		PaletteColor3f(ORANGE);
		// Draw the orange line
		glVertex3f(left + linePos, top, z);
		glVertex3f(left + linePos + notchWidth, top, z);
		glVertex3f(left + linePos + notchWidth, bottom, z);
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
		float dx = left - notchWidth/2.0f;
		for (int h = 1; h < rows; h++)
		{
			// Horizontal line
			mid = top - rowHeight * (float)h;
			glVertex3f(left, mid+lineWidth, z);
			glVertex3f(right, mid+lineWidth, z);
			glVertex3f(right, mid-lineWidth, z);
			glVertex3f(left, mid-lineWidth, z);

			short notches = 80;
			step = width / (float)notches;
			for (short s = 0; s <= notches; s += 1)
			{
				float h = notchHeight;
				if (s % 5 == 0)
				{
					h = notchLargeHeight;
				}
				glVertex3f(dx+s*step, mid-h, z);
				glVertex3f(dx+s*step+notchWidth, mid-h, z);
				glVertex3f(dx+s*step+notchWidth, mid+h, z);
				glVertex3f(dx+s*step, mid+h, z);
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
		if (names[ni].found
			&& names[ni].page == activeNamePage
			&& p.x < left+linePos)
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
			glTranslatef(crossSize, 0.0f, 0.0f);
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
Name TunerFx::CreateName(short page, std::string text)
{
	Name n;
	n.pos = glm::vec3(1000.0f, 0.0f, 0.0f);
	n.page = page;
	n.text = text;
	n.found = false;
	return n;
}
