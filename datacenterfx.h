#pragma once

#include "rocket/device.h"
#include "crossVector3.h"
#include "palette.h"
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
	void Quad(ColorName color, glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const glm::vec3& normal);
	float floorWidth;
	float floorDepth;

	float computerLight;
	float floorLight;
	float logoLight;
};
