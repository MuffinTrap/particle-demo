#pragma once

class FontGL;

class TunerFx
{
public:
	void Init();
	void Update(float deltaTime);
	void Draw(FontGL* font);

	float linePos = 0.0f;
};
