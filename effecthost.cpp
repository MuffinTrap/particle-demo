#include "effecthost.h"
#include <stdio.h>

EffectHost::EffectHost()
{

}

EffectHost::~EffectHost()
{
	radar.Quit();
	plotter.Quit();
}

void EffectHost::Init()
{
	printf("effecthost::Init()\n");
	font.LoadFromImage("andvari30x30.png", 30, 30, ' ');
    radar.Init(64);
	tuner.Init(640, 480);
	plotter.Init(32);
	activeEffect = 0.0f;
}


void EffectHost::Update ( float deltaTime )
{
	static float phase = 0.0f;

	if (activeEffect <= 1.0f)
	{
		radar.Update(deltaTime);
	}
	else if (activeEffect <= 2.0f)
	{
		tuner.Update(deltaTime);
	}
	else
	{
		phase += deltaTime;
		plotter.SetPhase(phase);
	}
}

void EffectHost::Draw()
{
	if (activeEffect <= 1.0f)
	{
		radar.Draw(&font);
	}
	else if (activeEffect <= 2.0f)
	{
		tuner.Draw(&font);
	}
	else
	{
		plotter.Draw();
	}
}

