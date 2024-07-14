#include "effecthost.h"
#include <wiiuse/wpad.h>
#include <mgdl-wii.h>
#include "mgdl-input-wii.h"

EffectHost::EffectHost()
{

}

EffectHost::~EffectHost()
{
	radar.Quit();
}

void EffectHost::Init()
{
	printf("effecthost::Init()\n");
	// NOTE: Andvari is 30x30
	//font.LoadFromBuffer(NES30x30_png, NES30x30_png_size, 30, 30, ' ' );
	font.LoadFromImage("andvari30x30.png", 30, 30, ' ');
	//font.LoadFromImage("font8x16.png", 8, 16, ' ');
	// font.LoadFromBuffer(font8x16_png, font8x16_png_size, 8, 16, ' ');
	//font.LoadFromBuffer(font_test_png, font_test_png_size, 8, 16, ' ');
	printf("Radar init\n");
    radar.Init(64);
	printf("Tuner init\n");
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

