#pragma once
#include <mgdl-wii.h>

class DebugCamera
{
public:
	void Draw3DAxii();
	void Update(float deltaTime);
	void ApplyMatrix();
	void Reset();
	guVector eye = {0.0f, 0.0f, 1.0f};
	guVector dir = {0.0f, 0.0f, -1.0f};
	guVector lookTarget = {0.0f, 0.0f, 0.0f};
	float cameraSpeed = 2.4f;
	float turnSpeedRadians = 0.8f;
};
