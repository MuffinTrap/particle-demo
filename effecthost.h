#pragma once
#include "radarfx.h"
#include "tunerfx.h"
#include "FontGL.h"

namespace gdl
{
	class Image;
};

enum EffectName
{
	Radar,
	Tuner
};

class EffectHost
{
public:
	EffectHost();
	~EffectHost();
	void Init();
	void Update(float deltaTime);
	void Draw();

	EffectName activeEffect;
	RadarFX radar;
	TunerFx tuner;

	FontGL font;
};
