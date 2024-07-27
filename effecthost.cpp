#include "effecthost.h"
#include "crossUtil.h"
#include <stdio.h>
#include <cmath>

#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *effect_active;  // Which effect is active.
#else
#include "src/sync_data.h"
#endif

EffectHost::EffectHost()
{

}

void EffectHost::Quit()
{
	radar.Quit();
	tuner.Quit();
	plotter.Quit();
	credits.Quit();
}

void EffectHost::Init(sync_device* rocket)
{
	font.LoadFromImage("andvari30x30.png", 30, 30, ' ');
    radar.Init(64, rocket);
	tuner.Init(640.0f/480.0f, rocket);
	plotter.Init(32);
	credits.Init(rocket);

	effect_active = sync_get_track(rocket, "effect_active");
	activeEffect = clampF(sync_get_val(effect_active, 0), 0.0f, 3.0f);

	printf("host init done\n");
}


void EffectHost::Update ()
{
	activeEffect = sync_get_val(effect_active, get_row());

	int ac = (int)floor(activeEffect);

	switch(ac)
	{
		case 1:
			radar.Update();
		break;
		case 2:
			tuner.Update();
		break;
	};
}

void EffectHost::Draw()
{
	int ac = (int)activeEffect;

	switch(ac)
	{
		case 1:
		radar.Draw(&font);
		break;
		case 2:
		tuner.Draw(&font);
		break;
		case 3:
		plotter.Draw();
		break;
		case 4:
		credits.Draw(&font);
		break;
	};
}

