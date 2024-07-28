#pragma once
#include "radarfx.h"
#include "tunerfx.h"
#include "FontGL.h"
#include "plotterfx.h"
#include "rocket/sync.h"

enum EffectName
{
	fxParticles = 0,
	fxRadar = 1,
	fxTuner = 2,
	fxPlotter = 3,
	fxCredits = 4
};

class EffectHost
{
public:
	EffectHost();
	void Init(sync_device* rocket);
	void Update();
	void Draw();
	void Quit();

	EffectName activeEffect;
	RadarFX radar;
	TunerFx tuner;
	PlotterFX plotter;

	FontGL font;
};
