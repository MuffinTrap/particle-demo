#include "palette.h"
#include <GL/opengx.h>
#include <mgdl-wii.h>

struct Col
{
	float r,g,b;
};

static Col Black = {0.133f, 0.137f, 0.137f};
static Col Grey = {0.612f, 0.616f, 0.612f};
static Col White = {0.941f, 0.965f, 0.941f};
static Col Orange = {0.941f, 0.502f, 0.137f};

void PaletteClearColor3f ( ColorName name )
{
	switch(name)
	{
		case ColorName::BLACK:
			glClearColor(Black.r, Black.g, Black.b, 0.0f);
			break;
		case ColorName::GREY:
			glClearColor(Grey.r, Grey.g, Grey.b, 0.0f);
			break;
		case ColorName::WHITE:
			glClearColor(White.r, White.g, White.b, 0.0f);
			break;
		case ColorName::ORANGE:
			glClearColor(Orange.r, Orange.g, Orange.b, 0.0f);
			break;
	};
}

void PaletteColor3f ( ColorName name )
{
	switch(name)
	{
		case ColorName::BLACK:
			glColor3f(Black.r, Black.g, Black.b);
			break;
		case ColorName::GREY:
			glColor3f(Grey.r, Grey.g, Grey.b);
			break;
		case ColorName::WHITE:
			glColor3f(White.r, White.g, White.b);
			break;
		case ColorName::ORANGE:
			glColor3f(Orange.r, Orange.g, Orange.b);
			break;

	};
}
void PaletteColor4f ( ColorName name, float alpha)
{
	switch(name)
	{
		case ColorName::BLACK:
			glColor4f(Black.r, Black.g, Black.b, alpha);
			break;
		case ColorName::GREY:
			glColor4f(Grey.r, Grey.g, Grey.b, alpha);
			break;
		case ColorName::WHITE:
			glColor4f(White.r, White.g, White.b, alpha);
			break;
		case ColorName::ORANGE:
			glColor4f(Orange.r, Orange.g, Orange.b, alpha);
			break;
	};
}
