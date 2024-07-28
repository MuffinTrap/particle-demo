#pragma once

#include "rocket/device.h"
#include "FontGL.h"
#include <string>
#include <vector>

class CreditsFX
{
public:
	void Init(sync_device* rocket);
	void Update();
	void Draw(FontGL *font);
	void Quit();

	std::vector<std::string> names;

};
