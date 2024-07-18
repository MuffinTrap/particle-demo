#pragma once
#include "radarfx.h"
#include "tunerfx.h"
#include "FontGL.h"
#include "plotterfx.h"

namespace gdl
{
	class Image;
};


class EffectHost
{
public:
	EffectHost();
	~EffectHost();
	void Init();
	void Update(float deltaTime);
	void Draw();

	float activeEffect;
	RadarFX radar;
	TunerFx tuner;
	PlotterFX plotter;

	FontGL font;
};
