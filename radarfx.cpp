#include "radarfx.h"

#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "crossOpenGL.h"
#include "crossCache.h"
#include "crossRandom.h"
#include "crossUtil.h"

#include "FontGL.h"
#include "palette.h"
#include "platform.h"

#include "rocket/track.h"

#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *radar_dots;  // How many dots to draw
    const struct sync_track *radar_seed;  // The random seed for dot positions and +/- marks. Change causes dot place randomization
    const struct sync_track *radar_rarity;  // One in a "radar_rarity" change of drawing + or - mark over dot and printing values
    const struct sync_track *radar_spread;  // How scattered the dots are from origo: max 1.0. Change causes dot place randomization
#else
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

RadarFX::RadarFX()
{
	gridSize = 16;
	dotsArray = nullptr;
	drawAmount = 0;
	seed =100;
	elapsed = 0.0f;
	dotSpread = 0.5f;
	rarity = 4;

	// How far away the text is from the side of the grid relative to width;
	gap = 1.0f/8.0f;

	// How big text relative to width;
	textHeightRatio = 1.0f/32.0f;

	// Size of chevrons and +- symbols. Relative to cell size;
	chevronWidthRatio = 1.0f/6.0f;
	chevronLengthRatio = 1.0f/3.0f;

	symbolRatio = 1.0f/6.0f;

}

void RadarFX::Init ( u32 dotAmount, sync_device* rocket )
{
	this->dotAmount = dotAmount;
	numberEntries.reserve(dotAmount);

	size_t dotArraySize = dotAmount * sizeof(glm::vec3);
	dotsArray = (glm::vec3*)malloc(dotArraySize);

#ifndef SYNC_PLAYER
	radar_dots = sync_get_track(rocket, "radar_dots");
	radar_seed = sync_get_track(rocket, "radar_seed");
	radar_spread = sync_get_track(rocket, "radar_spread");
	radar_rarity = sync_get_track(rocket, "radar_rarity");
#endif

	rarity = static_cast<u32>(floor(sync_get_val(radar_rarity, 0)));
	dotSpread = clampF(sync_get_val(radar_spread, 0), 0.0f, 1.0f) / 2.0f;
	seed = static_cast<u32>(sync_get_val(radar_seed, 0));
	srand(seed);

	RandomizeDots();
}

void RadarFX::Quit()
{
#ifndef SYNC_PLAYER
	save_sync(radar_dots, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(radar_seed, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(radar_spread, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(radar_rarity, SYNC_FILE_H, SYNC_FILE_CPP);
#endif
	if (dotsArray != nullptr)
	{
		free(dotsArray);
	}
	dotsArray = nullptr;
}

void RadarFX::RandomizeDots()
{
	printf("radar randomize %u dots\n", dotAmount);
	for(u32 i = 0; i < dotAmount; i++)
	{
		dotsArray[i] = glm::vec3(0.5f + GetRandomFloat(-dotSpread, dotSpread), 0.5f + GetRandomFloat(-dotSpread, dotSpread), 0.0f);
	}
	CacheFlushRange(dotsArray, dotAmount * sizeof(glm::vec3));
	raresFound = 0;
	lastRareFound = -1;
	numberEntries.clear();
}



void RadarFX::Update ()
{
	double R = get_row();
	rarity = static_cast<u32>(floor(sync_get_val(radar_rarity, R)));

	float fAmount = floor(sync_get_val(radar_dots, R));
	u32 uAmount = static_cast<u32>(fAmount);
	drawAmount = clampU32( uAmount, 0, dotAmount);

	bool newDots = false;
	u32 newSeed = (u32)floor(sync_get_val(radar_seed, R));
	if (newSeed != seed)
	{
		seed = newSeed;
		srand(seed);
		newDots = true;
	}

	float newSpread = (float)sync_get_val(radar_spread, R);
	if (newSpread != dotSpread)
	{
		dotSpread = newSpread;
		newDots = true;
	}

	if (newDots)
	{
		RandomizeDots();
	}
}


void RadarFX::DrawGrid(float left, float right, float top, float bottom)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	// Draw the grid
	// TODO
	// Bold white cross in the origo
	// Chevrons in corners and sides
	// zero axes with bright grey
	// grid with light transparent grey (crossings are a bit lighter than lines)
	glBegin(GL_LINES);

	// Grey grid
	PaletteColor3f(GREY);
	for (u32 c = 0; c <= gridSize; c++)
	{
		// horizontal line
		glVertex3f(left, top - c * cellSize, z);
		glVertex3f(right, top - c * cellSize, z);

		// Vertical line
		glVertex3f(left + c * cellSize, top, z);
		glVertex3f(left + c * cellSize, bottom, z);
	}

	// Borders
	// Vertical axis
	glVertex3f(left, y, z);
	glVertex3f(right, y, z);

	// horizontal axis
	glVertex3f(x, top, z);
	glVertex3f(x, bottom, z);

	// top
	glVertex3f(left, top, z);
	glVertex3f(right, top, z);
	// bottom
	glVertex3f(left, bottom, z);
	glVertex3f(right, bottom, z);

	// left
	glVertex3f(left, top, z);
	glVertex3f(left, bottom, z);
	// right
	glVertex3f(right, top, z);
	glVertex3f(right, bottom, z);

	glEnd();

	// Center cross and chevrons in white
	float c2= cellSize * chevronLengthRatio / 2.0f;
	float c8= cellSize * chevronLengthRatio;
	glBegin(GL_QUADS);
	PaletteColor3f(WHITE);

	// Center
	glVertex3f(x-c2, y+c8, z);
	glVertex3f(x+c2, y+c8, z);
	glVertex3f(x+c2, y-c8, z);
	glVertex3f(x-c2, y-c8, z);

	glVertex3f(x-c8, y+c2, z);
	glVertex3f(x+c8, y+c2, z);
	glVertex3f(x+c8, y-c2, z);
	glVertex3f(x-c8, y-c2, z);

	// Top left
	glVertex3f(left-c8, top+c8, z);
	glVertex3f(left+c2, top+c8, z);
	glVertex3f(left+c2, top-c8, z);
	glVertex3f(left-c8, top-c8, z);

	glVertex3f(left-c8, top+c8, z);
	glVertex3f(left+c8, top+c8, z);
	glVertex3f(left+c8, top-c2, z);
	glVertex3f(left-c8, top-c2, z);

	// Top right
	glVertex3f(right-c2, top+c8, z);
	glVertex3f(right+c8, top+c8, z);
	glVertex3f(right+c8, top-c8, z);
	glVertex3f(right-c2, top-c8, z);

	glVertex3f(right-c8, top+c8, z);
	glVertex3f(right+c8, top+c8, z);
	glVertex3f(right+c8, top-c2, z);
	glVertex3f(right-c8, top-c2, z);

	// Bottom right
	glVertex3f(right-c2, bottom+c8, z);
	glVertex3f(right+c8, bottom+c8, z);
	glVertex3f(right+c8, bottom-c8, z);
	glVertex3f(right-c2, bottom-c8, z);

	glVertex3f(right-c8, bottom-c8, z);
	glVertex3f(right+c8, bottom-c8, z);
	glVertex3f(right+c8, bottom+c2, z);
	glVertex3f(right-c8, bottom+c2, z);

	// Bottom left
	glVertex3f(left-c8, bottom+c8, z);
	glVertex3f(left+c2, bottom+c8, z);
	glVertex3f(left+c2, bottom-c8, z);
	glVertex3f(left-c8, bottom-c8, z);

	glVertex3f(left-c8, bottom+c8, z);
	glVertex3f(left+c8, bottom+c8, z);
	glVertex3f(left+c8, bottom+c2, z);
	glVertex3f(left-c8, bottom+c2, z);

	glEnd();

}


static char numberbuffer[64];

void RadarFX::Draw(FontGL* font)
{
	float center_x = 0.0f;
	float center_y = 0.0f;
	float center_z = 0.0f;

	cellSize = 1.0f/(float)gridSize;
	float wh = 1.0f;
	float left = center_x - wh/2.0f;
	float right = left + wh;
	float top = center_y + wh/2.0f;
	float bottom = top - wh;

	float lineLength = wh * gap;

	DrawGrid(left, right, top, bottom);

	// Draw the dots/points
	float dz = cellSize/8.0f;
	glBegin(GL_QUADS);
		PaletteColor3f(WHITE);
		for(u32 di = 0; di < drawAmount; di++)
		{
			float dx = left + dotsArray[di].x * wh;
			float dy = top - dotsArray[di].y * wh ;
			glVertex3f(dx - dz, dy + dz, center_z);
			glVertex3f(dx + dz, dy + dz, center_z);
			glVertex3f(dx + dz, dy - dz, center_z);
			glVertex3f(dx - dz, dy - dz, center_z);
		}
	glEnd();

	// Draw lines

	// Lines start vertically from the center
	// Orange + or - on line start point
	float textHeight = wh *textHeightRatio;
	u32 leftUpIndex = 1;
	u32 leftDownIndex = 0;
	u32 rightUpIndex = 1;
	u32 rightDownIndex = 0;

	// Draw random thing the same way every time
	srand(seed);

	glBegin(GL_LINES);
	for(u32 di = 0; di < drawAmount; di++)
	{
		if (GetRandomInt(0, rarity) == 0)
		{
			if (di == drawAmount-1)
			{
				// Line is orange when it appears
				PaletteColor3f(ORANGE);
			}
			else
			{
				PaletteColor3f(GREY);
			}
			// Dot position

			float dotx = left + dotsArray[di].x * wh;
			float doty = top-dotsArray[di].y * wh;
			glVertex3f(dotx, doty, center_z);
			if (dotsArray[di].x < 0.5f)
			{
				// Line to the left
				float lineEndY = center_y;
				if (dotsArray[di].y < 0.5f)
				{
					lineEndY = center_y + (leftUpIndex) * textHeight;
					leftUpIndex++;
				}
				else
				{
					lineEndY = center_y - (leftDownIndex) * textHeight;
					leftDownIndex++;
				}
				float ydiff = lineEndY - doty;
				float lineCornerX = dotx - fabs(ydiff);
				float lineCornerY = lineEndY;
				float lineEndX = left-lineLength;

				if (lineCornerX < lineEndX)
				{
					// too far!
					lineCornerX = dotx;
				}

				glVertex3f(lineCornerX, lineCornerY, center_z);
				glVertex3f(lineCornerX, lineCornerY, center_z);
				glVertex3f(lineEndX, lineCornerY, center_z);

				// if this is a new rare, add it to entries
				if ((int)di > lastRareFound)
				{
					NumberEntry e;
					e.lineEnd = glm::vec3(lineEndX, lineEndY, center_z);
					sprintf(numberbuffer, "%.4f %.4f %.4f", fabs(e.lineEnd.x), fabs(e.lineEnd.y), e.lineEnd.z);
					e.text = numberbuffer;
					e.alignmentX = RJustify;
					numberEntries.push_back(e);
				}
			}
			else
			{
				float ry = center_y;
				if (dotsArray[di].y < 0.5f)
				{
					ry = center_y + (rightUpIndex) * textHeight;
					rightUpIndex++;
				}
				else
				{
					ry = center_y - (rightDownIndex) * textHeight;
					rightDownIndex++;
				}
				// Line to the right
				float ydiff = ry - doty;
				float lineCornerX = dotx + abs(ydiff);
				float lineEndX = right + lineLength;
				if (lineCornerX > lineEndX)
				{
					lineCornerX = dotx;
				}
				glVertex3f(lineCornerX, ry, center_z);
				glVertex3f(lineCornerX, ry, center_z);
				glVertex3f(lineEndX, ry, center_z);

				if ((int)di > lastRareFound)
				{
					NumberEntry e;
					e.lineEnd = glm::vec3(lineEndX, ry, center_z);
					snprintf(numberbuffer, 64, "%.4f %.4f %.4f", fabs(e.lineEnd.x), fabs(e.lineEnd.y), e.lineEnd.z);
					e.text = numberbuffer;
					e.alignmentX = LJustify;
					numberEntries.push_back(e);
				}
			}

			if ((int)di > lastRareFound)
			{
				// new rare point!
				// update last
				lastRareFound = (int)di;
				raresFound += 1;
			}
		}
	}
	glEnd();

	// Plus or minus symbols
	// Reset generator to get the marks on the same dots as lines
	srand(seed);

	float c2= cellSize * symbolRatio / 2.0f;
	float c8= cellSize * symbolRatio;

	glBegin(GL_QUADS);
	PaletteColor3f(ORANGE);
	bool plus = true;
	for(u32 di = 0; di < drawAmount; di++)
	{
		if (GetRandomInt(0,rarity)==0)
		{
			float px = left + dotsArray[di].x * wh;
			float py = top - dotsArray[di].y * wh;
			glVertex3f(px-c2, py+c8, center_z);
			glVertex3f(px+c2, py+c8, center_z);
			glVertex3f(px+c2, py-c8, center_z);
			glVertex3f(px-c2, py-c8, center_z);
			if (plus)
			{
				glVertex3f(px-c8, py+c2, center_z);
				glVertex3f(px+c8, py+c2, center_z);
				glVertex3f(px+c8, py-c2, center_z);
				glVertex3f(px-c8, py-c2, center_z);
			}
			plus = !plus;
		}
	}
	glEnd();

	// Draw text for each rare that has been found
	u32 visible = minU32(raresFound, drawAmount);
	u32 namedraws = minU32(visible, numberEntries.size());
	for (u32 ti = 0; ti < namedraws; ti++)
	{
		NumberEntry &e = numberEntries[ti];
		glPushMatrix();
		glTranslatef(e.lineEnd.x, e.lineEnd.y, e.lineEnd.z);
			font->Printf(WHITE, textHeight, e.alignmentX, Centered, e.text.c_str());
		glPopMatrix();
	}
}
