#pragma once

// This class abstracts the platform
class Platform
{
public:

	void Init(int argc, char** argv, bool useRocket);
	void LoadMusic(const char* filename);
	void PlayMusic();
	void RunMainLoop();

	bool rocket_in_use = false;
};
