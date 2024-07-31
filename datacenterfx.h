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

	void DrawFloor();
	void DrawComputer(FontGL* font);
};
