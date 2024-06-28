#pragma once
#include <mgdl-wii.h>
#include "vehicle.h"

struct RGB8
{
	u8 r;
	u8 g;
	u8 b;
};

enum ParticleMode
{
	Seek,
	Rotate,
	SinWave,
	Loop
};

class ParticleCloud
{
	public:
		void Init(u32 amount, u32 color);
		void Draw();
		void Update(float deltaTime, ParticleMode mode);

		void Quit();

		void* listPtr = nullptr;
		u32 displayListSize = 0;
		guVector* positionsArray = nullptr;
		RGB8* colorsArray= nullptr;

		guVector target;
		u32 amountParticles;

		Vehicle* vehicleList;
		float elapsed = 0.0f;
		float interval = 1.0f;

		Mtx rotationMatrix;
		float rotationAngleDeg = 0.0f;
		float sinPhase = 0.0f;
};
