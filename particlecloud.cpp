#include "particlecloud.h"
#include <math.h>

void ParticleCloud::Init ( u32 amount, u32 color)
{
	amountParticles = amount;

	// Reserve size for positions
	positionsArray = (guVector*)malloc(sizeof(guVector) * amount);
	indicesArray = (GLushort*)malloc(sizeof(GLushort)*amount);
	for (u32 w = 0; w < amount; w++)
	{
		positionsArray[w] = {
			gdl::GetRandomFloat(-40, 40),   // X
			gdl::GetRandomFloat(-40, 40), 	// Y
			gdl::GetRandomFloat(-100, -10.0f)};
		indicesArray[w] = w;
	}

	float r = (float)RED(color)/255.0f;
	float g = (float)GREEN(color)/255.0f;
	float b = (float)BLUE(color)/255.0f;

	listIndex = glGenLists(1);
	glNewList(listIndex, GL_COMPILE);
		glPointSize(1);
		glColor3f(r, g, b);
		glVertexPointer(3, GL_FLOAT, 0, &positionsArray);
		glDrawElements(GL_POINTS, amount, GL_UNSIGNED_SHORT, indicesArray);
	glEndList();

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
}

void ParticleCloud::Quit()
{
	free(vehicleList);
	free(positionsArray);
	free(indicesArray);
}


void ParticleCloud::Draw()
{
	glPushMatrix();
	glRotatef(rotationAngleDeg, 0.0f, 0.0f, gdl::FORWARD.z);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
	glCallList(listIndex);
	glPopMatrix();
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

