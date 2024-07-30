#pragma once

#include "crossVector3.h"

class PlotterFX
{
public:
	void Init(int lineAmount);
	void SetPhase(float phase);
	void Draw();
	void Save();
	void Free();

private:
	void GeneratePoints(int width, int depth);
	glm::vec3* points;
	int width;
	int depth;
	float plasmaPhase;

};
