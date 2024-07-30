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
	void Update();
	bool ConnectRocket();
	void Draw();
	void Free();
	void Save();
	void UpdateController(WiiController& controllerState);
	bool QuitRequested();

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
