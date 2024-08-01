#pragma once

class FontGL;

class RocketDebug
{
	public:
	void Init();
	void DrawFramerate(FontGL* font);
	void DrawThirds();
	void Free();
};
