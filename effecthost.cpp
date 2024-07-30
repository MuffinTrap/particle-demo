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
	const struct sync_track *SdfType; // what kind of sdf?
#else
    // This is for the wii version.
    // Read track data from a header

    // NOTE
    // I spent over an hour trying to figure this one out
    // If the .cpp file is not included, the linker does not find the tracks.
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

EffectHost::EffectHost()
{

}

void EffectHost::Init(sync_device* rocket)
{
	font.LoadFromImage("data/andvari62x62CAPS.png", 62, 62, ' ');

	title.Init(rocket);
    radar.Init(64, rocket);
	tuner.Init(640.0f/480.0f, rocket);
	plotter.Init(32);
	credits.Init(rocket);


	rocketDebug.Init();

#ifndef SYNC_PLAYER
	effect_active = sync_get_track(rocket, "effect_active");
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
	SdfType = sync_get_track(rocket, "SdfType");
#endif

	activeEffect = fxTitle;

	init_perlin(0x1337);  // a LEET number
	initParticles();
	printf("host init done\n");
}

void EffectHost::Quit()
{
#ifndef SYNC_PLAYER
	save_sync(effect_active, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_rotX, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_rotY, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_rotZ, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_X, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_Y, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_Z, SYNC_FILE_H, SYNC_FILE_CPP);

	save_sync(OrbitX, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(OrbitY, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(OrbitZ, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(Repulsion, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(RepulsionPower, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(Gravity, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(GravityPower, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(Friction, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(EffectA, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(EffectB, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(EffectC, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(EffectD, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(EffectE, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(EffectF, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(WindX, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(WindY, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(WindZ, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(ParticleCount, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(SdfType, SYNC_FILE_H, SYNC_FILE_CPP);
#endif

	title.Quit();
	radar.Quit();
	tuner.Quit();
	plotter.Quit();
	credits.Quit();

	rocketDebug.Quit();
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
			uniform_SdfType = sync_get_val(SdfType, R);

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

		case fxQUIT_DEMO:
			// NOP
		break;
	};

	glPopMatrix();
	glPopMatrix();

#ifdef PROFILING
	rocketDebug.Draw(&font);
#endif
}

