#include "radarfx.h"
#include <GL/opengx.h>
#include <stdlib.h>

RadarFX::RadarFX()
{
	centerPosition = {0.0f, 0.0f, 0.0f};
	gridSize = 16;
	cellSize = 1.0f;
}

void RadarFX::Init ( u32 dotAmount )
{
	this->dotAmount = dotAmount;
	dotsArray = (guVector*)malloc(sizeof(guVector)*dotAmount);
	for(u32 i = 0; i < dotAmount; i++)
	{
		dotsArray[i] = guVector{gdl::GetRandomFloat(0.0f, 1.0f), gdl::GetRandomFloat(0.0f, 1.0f), 0.0f};
	}

	// TODO Sort the dots so that it start from the middle of y
}

void RadarFX::Quit()
{
	if (dotsArray != nullptr)
	{
		free(dotsArray);
	}
	dotsArray = nullptr;
}


void RadarFX::Update ( float deltaTime )
{
	elapsed += deltaTime;
	if (elapsed > 0.15f)
	{
		elapsed = 0.0f;
		drawAmount++;
		if (drawAmount == dotAmount)
		{
			drawAmount = 0;
		}
	}
}


void RadarFX::DrawGrid(float left, float right, float top, float bottom)
{
	float x = centerPosition.x;
	float y = centerPosition.y;
	float z = centerPosition.z;

	// Draw the grid
	// TODO
	// Bold white cross in the origo
	// Chevrons in corners and sides
	// zero axes with bright grey
	// grid with light transparent grey (crossings are a bit lighter then lines)
	glBegin(GL_LINES);
	glColor3f(0.3f, 0.3f, 0.3f);
	for (u32 c = 0; c <= gridSize; c++)
	{
		// horizontal line
		glVertex3f(left, top - c * cellSize, z);
		glVertex3f(right, top - c * cellSize, z);

		// Vertical line
		glVertex3f(left + c * cellSize, top, z);
		glVertex3f(left + c * cellSize, bottom, z);
	}
	glColor3f(0.6f, 0.6f, 0.6f);
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
	// Center cross and chevrons
	float c2= cellSize/4.0f;
	float c8= cellSize/8.0f;
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

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



void RadarFX::Draw()
{
	float z = centerPosition.z;
	float x = centerPosition.x;
	float y = centerPosition.y;
	float wh = cellSize * gridSize;
	float left = x - wh/2.0f;
	float right = left + wh;
	float top = y + wh/2.0f;
	float bottom = top - wh;

	glPushMatrix();
	glScalef(0.25f, 0.25f, 1.0f);
	glTranslatef(-wh/2.0f, 0.0f, 0.0f);

	DrawGrid(left, right, top, bottom);

	// Draw the dots/points
	// TODO DrawArrays and increase amount with time
	float dz = cellSize/8.0f;
	glBegin(GL_QUADS);
	for(u32 di = 0; di < drawAmount; di++)
	{
		glVertex3f(left + dotsArray[di].x * wh - dz, top - dotsArray[di].y * wh + dz, z);
		glVertex3f(left + dotsArray[di].x * wh + dz, top - dotsArray[di].y * wh + dz, z);
		glVertex3f(left + dotsArray[di].x * wh + dz, top - dotsArray[di].y * wh - dz, z);
		glVertex3f(left + dotsArray[di].x * wh - dz, top - dotsArray[di].y * wh - dz, z);
	}

	glEnd();

	// Draw lines

	// TODO Lines start vertically from the center
	// Orange + or - on line start point
	// Line is orange when it appears
	float textHeight = wh / 32.0f;
	u32 leftUpIndex = 1;
	u32 leftDownIndex = 1;
	u32 rightUpIndex = 1;
	u32 rightDownIndex = 1;
	glBegin(GL_LINES);
	for(u32 di = 0; di < drawAmount; di++)
	{
		if (di % 4 == 0)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			if (di == drawAmount-1)
			{
				glColor3f(1.0f, 0.4f, 0.2f); // ORANGE
			}
			// Dot position
			float dotx = left + dotsArray[di].x * wh;
			float doty = top-dotsArray[di].y * wh;
			glVertex3f(dotx, doty, z);
			if (dotsArray[di].x < 0.5f)
			{
				// Line to the left
				// TODO Lines in 45 angle only
				float ly = y;
				if (dotsArray[di].y < 0.5f)
				{
					ly = y + (leftUpIndex) * textHeight;
					leftUpIndex++;
				}
				else
				{
					ly = y - (leftDownIndex) * textHeight;
					leftDownIndex++;
				}
				float ydiff = ly - doty;
				float lx = dotx - abs(ydiff);
				glVertex3f(lx, ly, z);

				glVertex3f(lx, ly, z);
				glVertex3f(left - wh/2, ly, z);
			}
			else
			{
				float ry = y;
				if (dotsArray[di].y < 0.5f)
				{
					ry = y + (rightUpIndex) * textHeight;
					rightUpIndex++;
				}
				else
				{
					ry = y - (rightDownIndex) * textHeight;
					rightDownIndex++;
				}
				// Line to the right
				float ydiff = ry - doty;
				float rx = dotx + abs(ydiff);
				glVertex3f(rx, ry, z);

				glVertex3f(rx, ry, z);
				glVertex3f(right + wh/2, ry, z);
			}
		}
	}
	glEnd();


	// Center cross and chevrons
	float c2= cellSize/4.0f;
	float c8= cellSize/8.0f;
	// Plus or minus
	glColor3f(1.0f, 0.4f, 0.2f); // ORANGE

	glBegin(GL_QUADS);
	bool plus = true;
	for(u32 di = 0; di < drawAmount; di++)
	{
		if (di % 4 == 0)
		{
			float px = left + dotsArray[di].x * wh;
			float py = top - dotsArray[di].y * wh;
			glVertex3f(px-c2, py+c8, z);
			glVertex3f(px+c2, py+c8, z);
			glVertex3f(px+c2, py-c8, z);
			glVertex3f(px-c2, py-c8, z);
			if (plus)
			{
				glVertex3f(px-c8, py+c2, z);
				glVertex3f(px+c8, py+c2, z);
				glVertex3f(px+c8, py-c2, z);
				glVertex3f(px-c8, py-c2, z);
			}
			plus = !plus;
		}
	}
	glEnd();

	// Draw texts
	// TODO Three words for each line

	glPopMatrix();

}
