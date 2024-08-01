#pragma once

#include "rocket/device.h"
class FontGL;

class DataCenterFX
{
public:
	void Init(sync_device* rocket);
	void Update();
	void Draw(FontGL* font);
	void Save();

	void DrawComputers(FontGL* font);
	void DrawFloor(float wide, float deep);
	void DrawComputer(FontGL* font, short number);

private:
	float floorWidth;
	float floorDepth;

	float computerLight;
	float floorLight;
	float logoLight;
};
