#include "vehicle.h"
#include <mgdl-wii/mgdl-vector-operations.h>

Vehicle::Vehicle()
{
	position = {0,0,0};
	velocity = {0,0,0};
	acceleration = {0,0,0};
	target = {0,0,0};
}


void Vehicle::Update ( float deltaTime )
{
	velocity = velocity + (acceleration * deltaTime);
	velocity = guVecLimit(velocity, maxSpeed);

	position = position + velocity * deltaTime;

	guVecZero(acceleration);
}

void Vehicle::AddForce ( guVector force )
{
	acceleration = acceleration + force;
}

// ???
static float map(float value, float minValue, float maxValue, float lowLimit, float maxLimit)
{
	float limitRange = maxLimit - lowLimit;
	float valueRange = maxValue - minValue;

	return lowLimit + (limitRange / valueRange) * (value - minValue);
}

// Same as seek, but slows down when less then arriveDistance from target
void Vehicle::Arrive ( guVector target, float forceWeight)
{
	guVector desired = (target + targetOffset) - position;
	float d = guVecMagnitude(desired);
	guVecNormalize(&desired);

	if (d < arriveDistance)
	{
		float m = map(d, 0.0f, arriveDistance, 0.0f, maxSpeed);
		desired *= m;
	}
	else
	{
		desired *= maxSpeed;
	}

	guVector steer = desired - velocity;

	// if steer magnitude is larger than maxforce
	guVecLimit(steer, maxForce);
	AddForce(steer * forceWeight);
}


guVector Vehicle::Seek ( guVector target )
{
	this->target = target;
	guVector desired = target - position;
	guVecNormalize(&desired);
	desired *= maxSpeed;

	guVector steer = desired - velocity;

	// if steer magnitude is larger than maxforce
	guVecLimit(steer, maxForce);
	return steer;
}


void Vehicle::Flock (std::vector<Vehicle>& others,
					 float separationDistance, float cohesionDistance, float alignmentDistance,
					 float separationWeight, float cohesionWeight , float aligmentWeight )
{
	guVector separation = Separate(others, separationDistance);
	guVector cohesion = Cohesion(others, cohesionDistance);
	guVector alignment = Align(others, alignmentDistance);

	separation *= separationWeight;
	cohesion *= cohesionWeight;
	alignment *= aligmentWeight;

	AddForce(separation);
	AddForce(cohesion);
	AddForce(alignment);
}

// Avoid other vehicles that are inside given separation distance
guVector Vehicle::Separate ( std::vector<Vehicle>& others, float distance )
{
	float separationDistance = distance;
	int count = 0;
	guVector sum = {0,0,0};
	for (size_t i = 0; i < others.size(); i++)
	{
		guVector fromOther = position - others[i].position;
		float distance = guVecMagnitude(fromOther);

		// Must be greator to zero so will not avoid itself :D
		if (distance > 0 && distance < separationDistance)
		{
			guVecNormalize(&fromOther);

			// Flee more when closer
			fromOther /= distance;

			sum =  sum + fromOther;
			count++;
		}
	}
	if (count > 0)
	{
		sum /= (float)count;
		guVecNormalize(&sum);
		sum *= maxSpeed;
		guVector steer = sum - velocity;

		// if steer magnitude is larger than maxforce
		guVecLimit(steer, maxForce);
		return steer;
	}
	else
	{
		return sum;
	}
}

// Get closer to other vehicles
guVector Vehicle::Cohesion ( std::vector<Vehicle>& others, float distance )
{
	float cohesionDistance = distance;
	int count = 0;
	guVector sum = {0,0,0};
	for (size_t i = 0; i < others.size(); i++)
	{
		guVector toOther = others[i].position - position;
		float distance = guVecMagnitude(toOther);
		// Must be greator to zero so will not go toward itself
		if (distance > 0 && distance < cohesionDistance)
		{
			sum = sum + others[i].position;
			count++;
		}
	}
	if (count > 0)
	{
		// Average position of others
		sum /= (float)count;
		return Seek(sum);
	}
	else {
		return {0,0,0};
	}
}

guVector Vehicle::Align ( std::vector<Vehicle>& others, float distance )
{
	guVector sum = {0,0,0};
	int count = 0;
	for (size_t i = 0; i < others.size(); i++)
	{
		guVector toOther = (others[i].position - position);
		float mag = guVecMagnitude(toOther);
		if (mag > 0 && mag < distance)
		{
			sum = sum + others[i].velocity;
			count++;
		}
	}
	if (count > 0)
	{
		sum /= (float)count;
		guVecNormalize(&sum);
		sum *= maxSpeed;
		guVector steer = sum - velocity;
		guVecLimit(steer, maxForce);
		return steer;
	}
	else
	{
		return sum;
	}
}






