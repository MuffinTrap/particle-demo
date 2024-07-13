#include "effecthost.h"
#include "mgdl-input-wii.h"
#include "wiiuse/wpad.h"



#include "andvari_png.h"


void EffectHost::Init()
{
	font.LoadFromImage("font8x16.png", 8, 16, ' ');
    radar.Init(64);
	tuner.Init();
	activeEffect = Radar;
}


void EffectHost::Update ( float deltaTime )
{
	if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_1))
	{
		activeEffect = Radar;
	}
	if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_2))
	{
		activeEffect = Tuner;
	}

	switch(activeEffect)
	{
		case Radar:
			radar.Update(deltaTime);
			break;
		case Tuner:
			tuner.Update(deltaTime);
			break;
	};
}

void EffectHost::Draw()
{
	switch(activeEffect)
	{
		case Radar:
			radar.Draw(&font);
			break;
		case Tuner:
			tuner.Draw(&font);
			break;
	};
}

