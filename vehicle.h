#pragma once
#include <mgdl-wii.h>
#include <vector>

// Vehicle class from Nature of Code, chapter 6
class Vehicle
{
	public:
		Vehicle();
		guVector position = {0,0,0};
		guVector velocity = {0,0,0};
		guVector acceleration = {0,0,0};
		guVector target = {0,0,0};
		guVector targetOffset = {0,0,0};

		float maxSpeed = 1.0f;
		float maxForce = 1.0f; // How fast can change direction

		float arriveDistance = 1.0f;

		// Physics
		void Update(float deltaTime);
		void AddForce(guVector force);

		// Behaviours
		guVector Seek(guVector target);
		void Arrive(guVector target, float forceWeight);
		void Flock(std::vector<Vehicle>& others,
				   float separationDistance, float cohesionDistance, float alignmentDistance,
					float separationWeight = 1.0f, float cohesionWeight = 1.0f, float aligmentWeight = 1.0f);

		// used by Flock
		guVector Align(std::vector<Vehicle>& others, float distance);
		guVector Separate(std::vector<Vehicle>& others, float distance);
		guVector Cohesion(std::vector<Vehicle>& others, float distance);


		// For drawing sprites
		int spriteIndex = 0;

};
