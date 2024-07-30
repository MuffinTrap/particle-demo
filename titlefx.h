#pragma once

class FontGL;
struct sync_device;

class TitleFX
{
public:
	void Init(sync_device* rocket);
	void Update();
	void Draw(FontGL* font);
	void Save();
};
