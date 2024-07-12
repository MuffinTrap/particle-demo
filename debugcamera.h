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
	float cameraSpeed = 3.0f;
};
