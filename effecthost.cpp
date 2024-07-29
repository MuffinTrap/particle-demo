#include "effecthost.h"
#include "crossUtil.h"
#include <stdio.h>
#include <cmath>

#include "rocket/track.h"

#include "src/direction.hpp"
#include "src/particles.h"
#ifndef SYNC_PLAYER
    const struct sync_track *effect_active;  // Which effect is active.
    const struct sync_track *scene_rotX;  // Rotation around X for all scenes
    const struct sync_track *scene_rotY;  // around Y
    const struct sync_track *scene_rotZ;  // around Z
    const struct sync_track *scene_X;  // Offset to Origo X
    const struct sync_track *scene_Y;  // Offset to Origo Y
    const struct sync_track *scene_Z;  // Offset to Origo Z

	const struct sync_track *OrbitX; // particle effect variable - against SDF
	const struct sync_track *OrbitY; // particle effect variable - against SDF
	const struct sync_track *OrbitZ; // particle effect variable - against SDF
	const struct sync_track *Repulsion; // particle effect variable - against SDF
	const struct sync_track *RepulsionPower; // particle effect variable - against SDF
	const struct sync_track *Gravity; // particle effect variable - against SDF
	const struct sync_track *GravityPower; // particle effect variable - against SDF
	const struct sync_track *Friction; // particle effect variable - against SDF
	const struct sync_track *EffectA; // particle effect variable - against SDF
	const struct sync_track *EffectB; // particle effect variable - against SDF
	const struct sync_track *EffectC; // particle effect variable - against SDF
	const struct sync_track *EffectD; // particle effect variable - against SDF
	const struct sync_track *EffectE; // particle effect variable - against SDF
	const struct sync_track *EffectF; // particle effect variable - against SDF
	const struct sync_track *WindX; // particle effect variable - against SDF
	const struct sync_track *WindY; // particle effect variable - against SDF
	const struct sync_track *WindZ; // particle effect variable - against SDF
	const struct sync_track *ParticleCount; // particle count total
#else
#include "src/sync_data.h"
#endif

EffectHost::EffectHost()
{

}

void EffectHost::Init(sync_device* rocket)
{
	font.LoadFromImage("andvari30x30.png", 30, 30, ' ');

	title.Init(rocket);
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

	OrbitX = sync_get_track(rocket, "OrbitX");
	OrbitY = sync_get_track(rocket, "OrbitY");
	OrbitZ = sync_get_track(rocket, "OrbitZ");
	Repulsion = sync_get_track(rocket, "Repulsion");
	RepulsionPower = sync_get_track(rocket, "RepulsionPower");
	Gravity = sync_get_track(rocket, "Gravity");
	GravityPower = sync_get_track(rocket, "GravityPower");
	Friction = sync_get_track(rocket, "Friction");
	EffectA = sync_get_track(rocket, "EffectA");
	EffectB = sync_get_track(rocket, "EffectB");
	EffectC = sync_get_track(rocket, "EffectC");
	EffectD = sync_get_track(rocket, "EffectD");
	EffectE = sync_get_track(rocket, "EffectE");
	EffectF = sync_get_track(rocket, "EffectF");
	WindX = sync_get_track(rocket, "WindX");
	WindY = sync_get_track(rocket, "WindY");
	WindZ = sync_get_track(rocket, "WindZ");
	ParticleCount = sync_get_track(rocket, "ParticleCount");

	init_perlin(0x1337);  // a LEET number
	initParticles();
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

	save_sync(OrbitX, "src/sync_data.h");
	save_sync(OrbitY, "src/sync_data.h");
	save_sync(OrbitZ, "src/sync_data.h");
	save_sync(Repulsion, "src/sync_data.h");
	save_sync(RepulsionPower, "src/sync_data.h");
	save_sync(Gravity, "src/sync_data.h");
	save_sync(GravityPower, "src/sync_data.h");
	save_sync(Friction, "src/sync_data.h");
	save_sync(EffectA, "src/sync_data.h");
	save_sync(EffectB, "src/sync_data.h");
	save_sync(EffectC, "src/sync_data.h");
	save_sync(EffectD, "src/sync_data.h");
	save_sync(EffectE, "src/sync_data.h");
	save_sync(EffectF, "src/sync_data.h");
	save_sync(WindX, "src/sync_data.h");
	save_sync(WindY, "src/sync_data.h");
	save_sync(WindZ, "src/sync_data.h");
	save_sync(ParticleCount, "src/save_sync");

	title.Quit();
	radar.Quit();
	tuner.Quit();
	plotter.Quit();
}


void EffectHost::Update ()
{
	activeEffect = static_cast<EffectName>(sync_get_val(effect_active, get_row()));

	float R = get_row();
	switch(activeEffect)
	{
		case fxParticles:
			uniform_OrbitX = sync_get_val(OrbitX, R);
			uniform_OrbitY = sync_get_val(OrbitY, R);
			uniform_OrbitZ = sync_get_val(OrbitZ, R);
			uniform_Repulsion = sync_get_val(Repulsion, R);
			uniform_RepulsionPower = sync_get_val(RepulsionPower, R);
			uniform_Gravity = sync_get_val(Gravity, R);
			uniform_GravityPower = sync_get_val(GravityPower, R);
			uniform_Friction = sync_get_val(Friction, R);
			uniform_EffectA = sync_get_val(EffectA, R);
			uniform_EffectB = sync_get_val(EffectB, R);
			uniform_EffectC = sync_get_val(EffectC, R);
			uniform_EffectD = sync_get_val(EffectD, R);
			uniform_EffectE = sync_get_val(EffectE, R);
			uniform_EffectF = sync_get_val(EffectF, R);
			uniform_WindX = sync_get_val(WindX, R);
			uniform_WindY = sync_get_val(WindY, R);
			uniform_WindZ = sync_get_val(WindZ, R);
			uniform_ParticleCount = sync_get_val(ParticleCount, R);
			updateParticles(1.0f/60.0f);
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
	glPushMatrix();
	float R = get_row();
	glTranslatef(sync_get_val(scene_X, R), sync_get_val(scene_Y, R), sync_get_val(scene_Z, R));

	glPushMatrix();
	glRotatef(sync_get_val(scene_rotX, R), 1.0f, 0.0f, 0.0f);
	glRotatef(sync_get_val(scene_rotY, R), 0.0f, 1.0f, 0.0f);
	glRotatef(sync_get_val(scene_rotZ, R), 0.0f, 0.0f, 1.0f);

	switch(activeEffect)
	{
		case fxTitle:
			title.Draw(&font);
			break;
		case fxParticles:
			displayParticles();
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
		case fxCredits:
			credits.Draw(&font);
		break;
		case fxRocketDebug:
			rocketDebug.Draw(&font);
		break;
	};

	glPopMatrix();
	glPopMatrix();
}

