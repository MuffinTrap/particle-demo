#pragma once
#include "crossVector3.h"
#include "wiicontroller.h"

class DebugCamera
{
public:
	void Init(int scrW, int scrH);
	void Draw3DAxii();
	void Update(float deltaTime, WiiController &controller);
	glm::vec3 NormalizedCursor(WiiController &controller);
	void ApplyMatrix();
	void Reset();
	glm::vec3 eye = {0.0f, 0.0f, 1.0f};
	glm::vec3 dir = {0.0f, 0.0f, -1.0f};
	glm::vec3 lookTarget = {0.0f, 0.0f, 0.0f};
	float cameraSpeed = 2.4f;
	float turnSpeedRadians = 0.8f;

	glm::vec3 normalizedCursor;

	int screenWidth = 1;
	int screenHeight = 1;
};
