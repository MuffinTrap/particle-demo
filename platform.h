#pragma once

#include "wiicontroller.h"

// This class abstracts the platform
class Platform
{
public:

	void Init(int argc, char** argv, bool useRocket);
	void LoadMusic(const char* filename, double beatsPerMin, int rowsPerBeat);
	void PlayMusic();
	void RunMainLoop();
	void ReadControllerInput(WiiController &controllerInOut);

	bool rocket_in_use = false;
};
