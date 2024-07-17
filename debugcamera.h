#pragma once
#include "crossVector3.h"
#include "wiicontroller.h"

class DebugCamera
{
public:
	void Init(int scrW, int scrH);
	void Draw3DAxii();
	void Update(float deltaTime, WiiController &controller);
	glm::vec2 NormalizedCursor(WiiController &controller);
	void ApplyMatrix();
	void Reset();
	glm::vec3 eye;
	glm::vec3 dir;
	glm::vec3 lookTarget;
	float cameraSpeed;
	float turnSpeedRadians;

	glm::vec2 normalizedCursor;

	int screenWidth;
	int screenHeight;
};
