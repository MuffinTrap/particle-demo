#pragma once

enum ColorName
{
	BLACK,
	GREY,
	WHITE,
	ORANGE
};

void PaletteClearColor3f(ColorName name);
void PaletteColor3f(ColorName name);
void PaletteColor4f(ColorName name, float alpha);
