#pragma once

#include "crossVector3.h"
#include "palette.h"
#include "rocket/sync.h"
#include "crossTypedefs.h"

class PlotterFX
{
public:
	void Init(int lineAmount, sync_device* rocket);
	void SetPhase(float phase);
	void Update();
	void Draw();
	void DrawBox();
	void DrawLines(ColorName colo, int skip);
	void DrawSurface();
	void Save();
	void Free();

	void ApplyPlasma();
	void ReadParticles();

private:
	void GeneratePoints ( int w, int d );
	glm::vec4* points;
	u32 width;
	u32 depth;
	float plasmaPhase;

	u32 frameCounter;

};
