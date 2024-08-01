#pragma once

#include <string>

class FontGL;
struct sync_device;

class TitleFX
{
public:
	void Init(sync_device* rocket);
	void Update();
	void Draw(FontGL* font);
	void DrawPattern(FontGL* font);
	void DrawName(FontGL* font);
	void DrawLogos(short amount);
	void Save();

	std::string crossPattern;
	std::string hatchPattern;
};
