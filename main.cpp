#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>

#include "mgdl-input-wii.h"
#include "particlecloud.h"
#include "quad.h"
#include "deltahistogram.h"

void init()
{
    fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes);
    gdl::SetClearColor(gdl::Color::Black);
    gdl::WiiInput::Init();
    gdl::ConsoleMode();
}

int main()
{
    init();

    gdl::ConsoleMode();

    // Test loading files
    gdl::Image font;
    gdl::FFont debugFont;
    font.LoadImage("font8x16.png", gdl::TextureFilterModes::Nearest, gdl::TextureFormatModes::RGBA8);
    debugFont.BindSheet(font, 8, 16, ' ');

    // Init your Game here
    /*
             Uncomment to see console messages
             before game starts
    */
    ParticleCloud cloudM;
    ParticleCloud cloudC;
    ParticleCloud cloudY;
    int size = 4096;
    cloudM.Init(size, gdl::Color::Magenta);
    cloudC.Init(size, gdl::Color::Cyan);
    cloudY.Init(size, gdl::Color::Yellow);

    gdl::Perspective::InitDefaultCamera();


    u64 deltaTimeStart = gettime();
    //u64 programStart = gettime();
    float deltaTime = 0.0f;
    // float mainElapsed = 0.0f;

    DeltaHistogram cpu = DeltaHistogram();

	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32,0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);


    while(true)
    {
        gdl::WiiInput::StartFrame();
        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            break;
        }
        VIDEO_WaitVSync();
    }

    // Update once to get all values set
    cloudM.Update(0.0f);
    cloudC.Update(0.0f);
    cloudY.Update(0.0f);

    bool gameRunning = true;
    while(true)
    {
        u64 now = gettime();
        deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
        deltaTimeStart = now;

        //mainElapsed= (float)(now- programStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds

        cpu.Update(gdl::Delta);
        gdl::WiiInput::StartFrame();

        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            gameRunning = false;
            // This starts the exit process
            gdl::wii::Exit();
        }


        if (gameRunning)
        {
            // Update your game here
            gdl::Camera& camera = gdl::Perspective::GetDefaultCamera();
            if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_UP))
            {
                camera.position.z += 1.0f;
            }
            if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_DOWN))
            {
                camera.position.z -= 1.0f;
            }
            if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_LEFT))
            {
                camera.position.z += 10.0f;
            }
            if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_RIGHT))
            {
                camera.position.z -= 10.0f;
            }
            cloudM.Update(deltaTime);
            // cloudC.Update(deltaTime);
            // cloudY.Update(deltaTime);
        }


        gdl::PrepDisplay();

        if (gameRunning)
        {
            gdl::Set3DMode(1000.0f);
            gdl::Perspective::ApplyDefaultCamera();
            cloudM.Draw();
            cloudC.Draw();
            cloudY.Draw();

            gdl::Set2DMode();
            cpu.Draw(gdl::ScreenXres-256, 16, 64, gdl::Color::Yellow);
            debugFont.Printf(gdl::ScreenXres-250, 18, 1.0f, gdl::Color::Black, "Delta");
        }
        gdl::Display();
    }
    cloudM.Quit();
    cloudC.Quit();
    cloudY.Quit();
}
