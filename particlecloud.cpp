#include "particlecloud.h"
#include <math.h>

void ParticleCloud::Init ( u32 amount, u32 color)
{
	amountParticles = amount;

	// Reserve size for positions
	positionsArray = (guVector*)aligned_alloc(32, sizeof(gdl::wii::VERT3f32) * amount);
	colorsArray = (RGB8*)aligned_alloc(32, sizeof(RGB8) * 4 );


	u32 endPadding = 32; // List must be 32 bytes bigger than needed
	u32 listSizePadded =
	sizeof(u8) + sizeof(u8) + sizeof(u16) + // GX_Begin parameters
	amount * sizeof(u16) +  // Position indices
	amount * sizeof(u16) +  // Color indices
	endPadding;

	listPtr = aligned_alloc(32, listSizePadded);
	DCInvalidateRange(listPtr, listSizePadded);

	for (u32 w = 0; w < amount; w++)
	{
		positionsArray[w] = {
			gdl::GetRandomFloat(-40, 40),   // X
			gdl::GetRandomFloat(-40, 40), 	// Y
			gdl::GetRandomFloat(-100, -10.0f)};
	}
	u8 r = RED(color);
	u8 g = GREEN(color);
	u8 b = BLUE(color);
	colorsArray[0] = RGB8{r, g, b};
	colorsArray[1] = RGB8{r, g, b};
	colorsArray[2] = RGB8{r, g, b};
	colorsArray[3] = RGB8{r, g, b};

	GX_BeginDispList(listPtr, listSizePadded);
	GX_Begin(GX_POINTS, GX_VTXFMT1, amount);
		for(u32 i = 0; i < amount; i++)
		{
			// Since the list is 32 byte aligned, both indices can be u16
			GX_Position1x16(i);
			GX_Color1x16(gdl::GetRandomInt(0,4)); // Change by speed?
		}
	GX_End();
	displayListSize = GX_EndDispList();
	gdl_assert(displayListSize > 0 && displayListSize <= listSizePadded, "Failed to create particleCloud displaylist: Result %u :  Param %u", displayListSize, listSizePadded);;
	printf("Created particle list size %u < %u\n", displayListSize, listSizePadded);

	GX_SetPointSize(16, 0);


	// Set up matching vehicles
	vehicleList = (Vehicle*)malloc(amount * sizeof(Vehicle));
	for (u32 i = 0; i < amount ; i ++)
	{
		vehicleList[i] = Vehicle();
		vehicleList[i].position = positionsArray[i]; // starting position, not changed
		vehicleList[i].maxForce = gdl::GetRandomFloat(20.0f, 40.0f);
		vehicleList[i].maxSpeed = gdl::GetRandomFloat(50.0f, 160.0f);
		vehicleList[i].arriveDistance = gdl::GetRandomFloat(1.0f, 5.0f);
		vehicleList[i].targetOffset =  guVector{gdl::GetRandomFloat(-4.0f, 4.0f), gdl::GetRandomFloat(-4.0f, 4.0f), gdl::GetRandomFloat(-4.0f, 4.0f)};
	}

	target = guVector{0.0f, 0.0f, -100.0f};
	elapsed = 0.0f;
	sinPhase = 0.0f;
	rotationAngleDeg = 0.0f;
	guMtxIdentity(rotationMatrix);
}

void ParticleCloud::Quit()
{
	free(vehicleList);
	free(listPtr);
	free(positionsArray);
	free(colorsArray);
}


void ParticleCloud::Draw()
{
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
	GX_SetArray(GX_VA_POS, (void*)positionsArray, sizeof(gdl::wii::VERT3f32));
	GX_SetArray(GX_VA_CLR0, (void*)colorsArray, sizeof(RGB8));

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	// Concat all matrices
	Mtx modelViewMatrix;
	//guMtxConcat(gdl::wii::ModelMtx, rotationMatrix, gdl::wii::ModelMtx);
	guMtxConcat(gdl::wii::ViewMtx, rotationMatrix, modelViewMatrix);
	GX_LoadPosMtxImm(modelViewMatrix, GX_PNMTX0);

	GX_CallDispList(listPtr, displayListSize);
}

void ParticleCloud::Update ( float deltaTime, ParticleMode mode )
{
	elapsed += deltaTime;

	switch (mode)
	{
		case ParticleMode::Seek:
			if (elapsed > interval )
			{
				target = guVector{gdl::GetRandomFloat(-60, 60), gdl::GetRandomFloat(-40, 40), gdl::GetRandomFloat(-100, -40.0f)};
				elapsed = 0.0f;
				interval = gdl::GetRandomFloat(1.0f, 3.0f);
			}
			for (u32 i = 0; i < amountParticles; i++)
			{
				// Go towards the target
				guVector position = positionsArray[i];
				Vehicle& v = vehicleList[i];
				guVector desired = (target + v.targetOffset) - position;
				guVecNormalize(&desired);
				desired *= v.maxSpeed;
				guVector steer = desired - v.velocity;

				guVecLimit(steer, v.maxForce);

				// Apply the acceleration and velocity
				vehicleList[i].acceleration = v.acceleration + steer;
				v.velocity = v.velocity + (v.acceleration * deltaTime);
				v.velocity = guVecLimit(v.velocity, v.maxSpeed);
				guVecZero(v.acceleration);

				positionsArray[i] = position + v.velocity * deltaTime;
			}
		break;
		case ParticleMode::Rotate:
		{
			rotationAngleDeg += 9.0f * deltaTime;
			guVector f = gdl::FORWARD;
			guMtxRotAxisDeg(rotationMatrix, &f, rotationAngleDeg);
		}
			break;
		case ParticleMode::SinWave:

			sinPhase += deltaTime;
			for (u32 i = 0; i < amountParticles; i++)
			{
				// Go towards the right side and loop around
				guVector position = positionsArray[i];
				Vehicle& v = vehicleList[i];
				position.y = v.position.y + sin(sinPhase + v.position.x) * (10.0f * v.maxForce/40.0f);
				position = position + gdl::RIGHT * deltaTime * 2.0f;
				if (position.x > 40.0f)
				{
					position.x = -40.0f;
				}
				positionsArray[i] = position;
			}
			break;
		case ParticleMode::Loop:
			for (u32 i = 0; i < amountParticles; i++)
			{
				// Go towards the right side and loop around
				guVector position = positionsArray[i];
				position = position + gdl::RIGHT * deltaTime;
				if (position.x > 40.0f)
				{
					position.x = -40.0f;
				}
				positionsArray[i] = position;
			}
			break;
	};


}

