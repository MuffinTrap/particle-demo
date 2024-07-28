#pragma once
#include "radarfx.h"
#include "tunerfx.h"
#include "FontGL.h"
#include "plotterfx.h"
#include "creditsfx.h"
#include "titlefx.h"
#include "rocket/sync.h"

enum EffectName
{
	fxTitle = 0,
	fxRadar = 1,
	fxTuner = 2,
	fxPlotter = 3,
	fxParticles = 4,
	fxCredits = 5
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
	TitleFX title;
	RadarFX radar;
	TunerFx tuner;
	PlotterFX plotter;
	CreditsFX credits;

	FontGL font;
};
