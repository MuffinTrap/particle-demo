#pragma once

#include "wiicontroller.h"

enum ScreenAspect
{
	Screen16x9,
	Screen4x3
};

// This class abstracts the platform
class Platform
{
public:

	void Init(int argc, char** argv, ScreenAspect aspectRatio, bool useRocket);
	void LoadMusic(const char* filename, double beatsPerMin, int rowsPerBeat);
	void PlayMusic();
	void RunMainLoop();

	static float GetDeltaTime();
	static float GetElapsedSeconds();
	static int GetScreenWidth();
	static int GetScreenHeight();

private:
	static int screenWidth;
	static int screenHeight;
};
