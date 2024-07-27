#pragma once
#include "radarfx.h"
#include "tunerfx.h"
#include "FontGL.h"
#include "plotterfx.h"
#include "rocket/sync.h"

namespace gdl
{
	class Image;
};


class EffectHost
{
public:
	EffectHost();
	void Init(sync_device* rocket);
	void Update();
	void Draw();
	void Quit();

	float activeEffect;
	RadarFX radar;
	TunerFx tuner;
	PlotterFX plotter;

	FontGL font;
};
