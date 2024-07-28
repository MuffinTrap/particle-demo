#include "effecthost.h"
#include "crossUtil.h"
#include <stdio.h>
#include <cmath>

#include "rocket/track.h"

#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *effect_active;  // Which effect is active.
    const struct sync_track *scene_rotX;  // Rotation around X for all scenes
    const struct sync_track *scene_rotY;  // around Y
    const struct sync_track *scene_rotZ;  // around Z
    const struct sync_track *scene_X;  // Offset to Origo X
    const struct sync_track *scene_Y;  // Offset to Origo Y
    const struct sync_track *scene_Z;  // Offset to Origo Z
#else
#include "src/sync_data.h"
#endif

EffectHost::EffectHost()
{

}

void EffectHost::Init(sync_device* rocket)
{
	font.LoadFromImage("andvari30x30.png", 30, 30, ' ');
    radar.Init(64, rocket);
	tuner.Init(640.0f/480.0f, rocket);
	plotter.Init(32);
	credits.Init(rocket);

	effect_active = sync_get_track(rocket, "effect_active");
	activeEffect = static_cast<EffectName>(clampF(sync_get_val(effect_active, 0), 0.0f, 3.0f));

	scene_rotX = sync_get_track(rocket, "scene_rotX");
	scene_rotY = sync_get_track(rocket, "scene_rotY");
	scene_rotZ = sync_get_track(rocket, "scene_rotZ");
	scene_X = sync_get_track(rocket, "scene_X");
	scene_Y = sync_get_track(rocket, "scene_Y");
	scene_Z = sync_get_track(rocket, "scene_Z");

	printf("host init done\n");
}

void EffectHost::Quit()
{
	save_sync(effect_active, "src/sync_data.h");
	save_sync(scene_rotX, "src/sync_data.h");
	save_sync(scene_rotY, "src/sync_data.h");
	save_sync(scene_rotZ, "src/sync_data.h");
	save_sync(scene_X, "src/sync_data.h");
	save_sync(scene_Y, "src/sync_data.h");
	save_sync(scene_Z, "src/sync_data.h");

	radar.Quit();
	tuner.Quit();
	plotter.Quit();
}


void EffectHost::Update ()
{
	activeEffect = static_cast<EffectName>(sync_get_val(effect_active, get_row()));

	switch(activeEffect)
	{
		case fxParticles:
			// TODO
			// Muffinhop, please put particle code update here
			break;
		case fxRadar:
			radar.Update();
		break;
		case fxTuner:
			tuner.Update();
		break;
		default:
			// Other effects don't need an update
			break;
	};
}

void EffectHost::Draw()
{
	switch(activeEffect)
	{
		case fxParticles:
			// TODO Muffinhop Please draw particle code here
			break;
		case fxRadar:
		radar.Draw(&font);
		break;
		case fxTuner:
		tuner.Draw(&font);
		break;
		case fxPlotter:
		plotter.Draw();
		break;
		case 4:
		credits.Draw(&font);
		break;
	};
}

