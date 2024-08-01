#include "effecthost.h"
#include "crossUtil.h"
#include <stdio.h>
#include <cmath>

#include "platform.h"
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

    const struct sync_track *fade_A;	// Fade alpha. 0 clear 1 black screen

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


// Pass the particles to the plotter
#include "src/particles.h"

EffectHost::EffectHost()
{

}

void EffectHost::Init(sync_device* rocket)
{
	// Needs to be power of 2 image
	font.LoadFromImage("apps/numbers_asm24/andvari32x32CAPS.png", 32, 32, ' ');

	title.Init(rocket);
    radar.Init(1024, rocket);
	tuner.Init((float)Platform::GetScreenWidth()/(float)Platform::GetScreenHeight(), rocket);
	plotter.Init(64, rocket);
	credits.Init(rocket);
	dataCenter.Init(rocket);


	rocketDebug.Init();

#ifndef SYNC_PLAYER
	effect_active = sync_get_track(rocket, "effect_active");
	scene_rotX = sync_get_track(rocket, "scene_rotX");
	scene_rotY = sync_get_track(rocket, "scene_rotY");
	scene_rotZ = sync_get_track(rocket, "scene_rotZ");
	scene_X = sync_get_track(rocket, "scene_X");
	scene_Y = sync_get_track(rocket, "scene_Y");
	scene_Z = sync_get_track(rocket, "scene_Z");
	fade_A = sync_get_track(rocket, "fade_A");

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

void EffectHost::Save()
{
#ifndef SYNC_PLAYER
	save_sync(effect_active, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_rotX, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_rotY, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_rotZ, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_X, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_Y, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(scene_Z, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(fade_A, SYNC_FILE_H, SYNC_FILE_CPP);

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

	title.Save();
	radar.Save();
	tuner.Save();
	plotter.Save();
	credits.Save();
	dataCenter.Save();
}

void EffectHost::Free()
{
	radar.Free();
	plotter.Free();
	rocketDebug.Free();
	freeParticles();
}

void EffectHost::Update ()
{
	activeEffect = static_cast<EffectName>(sync_get_val(effect_active, get_row()));

	switch(activeEffect)
	{
		case fxParticles:
			ParticleUpdate(Platform::GetDeltaTime());
			break;
		case fxRadar:
			radar.Update();
		break;
		case fxTuner:
			tuner.Update();
		break;
		case fxPlotter:
			ParticleUpdate(Platform::GetDeltaTime() * 0.05f);
			plotter.Update(particles, NUM_PARTICLES);
		break;
		case fxDataCenter:
			dataCenter.Update();
			break;
		default:
			// Other effects don't need an update
			break;
	};
}

// Separate so that other effects can also update particles when they want to
void EffectHost::ParticleUpdate(float delta)
{
	float R = get_row();
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

	updateParticles(delta);
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
		case fxDataCenter:
			dataCenter.Draw(&font);
			break;
		case fxQUIT_DEMO:
			// NOP
		break;
		default:
			// ROCKETDEBUG see below
			break;
	};

	glPopMatrix();
	glPopMatrix();

	float fade = sync_get_val(fade_A, R);
	if (fade > 0.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float W = Platform::GetScreenWidth();
		float H = Platform::GetScreenHeight();
		glOrtho(0, W, H, 0.0f, 1.0f, -1.0f);

		// Set the modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glBegin(GL_QUADS);
			PaletteColor4f(BLACK, fade);
			glVertex3f(0.0f, 0.0f, 0.9999f);
			glVertex3f(W, 0.0f, 0.9999f);
			glVertex3f(W, H, 0.9999f);
			glVertex3f(0.0f, H, 0.9999f);
		glEnd();
		glDisable(GL_BLEND);
	}

#ifdef PROFILING
	rocketDebug.DrawFramerate(&font);
#endif

#ifdef THIRDS
	rocketDebug.DrawThirds();
#endif
}

