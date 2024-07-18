#pragma once

#include "effecthost.h"
#include "debugcamera.h"
#include "wiicontroller.h"

// This is the cross platform demo class.
// It should not need to know what platform is in use.

class Demo
{
public:
	void Init(int scrW, int scrH, bool useRocket);
	void Update(float elapsed, float deltaTime);
	bool ConnectRocket();
	void Draw();
	void Quit();
	void UpdateController(WiiController& controllerState);

	int screenWidth;
	int screenHeight;
	float aspect;
	bool rocket_in_use;

	// Demo things
	float angle;

	EffectHost host;
	DebugCamera camera;
	WiiController controller;
};
