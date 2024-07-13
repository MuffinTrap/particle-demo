#pragma once
#include "radarfx.h"
#include "tunerfx.h"
#include "FontGL.h"

enum EffectName
{
	Radar,
	Tuner
};

class EffectHost
{
public:
	void Init();
	void Update(float deltaTime);
	void Draw();

	EffectName activeEffect;
	RadarFX radar;
	TunerFx tuner;

	FontGL font;
};
