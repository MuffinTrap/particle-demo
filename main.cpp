#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>

#include "mgdl-input-wii.h"
#include "particlecloud.h"
#include "quad.h"
#include "deltahistogram.h"
#include <GL/opengx.h>
#include <GL/glu.h>

void init()
{
    fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes, gdl::InitFlags::OpenGX);
    gdl::SetClearColor(gdl::Color::Black);
    gdl::WiiInput::Init();
    gdl::ConsoleMode();
}

void glRectf(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);
        glVertex3f(x1, y1, -1.0f);
        glVertex3f(x2, y1, -1.0f);
        glVertex3f(x2, y2, -1.0f);
        glVertex3f(x1, y2, -1.0f);
    glEnd();
}
int main()
{
    init();
    ogx_initialize();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_FLAT);
    glViewport(0, 0, gdl::ScreenXres, gdl::ScreenYres);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = (double)gdl::ScreenXres/(double)gdl::ScreenYres;


    // TODO How to set up the camera to look at particles???
    //glFrustum(aspect, aspect, -1.0, 1.0, 0.1f, 1000.0f);
    glOrtho(-40.0f * aspect, 40.0f * aspect, -40.0f, 40.0f, -0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Use arrays and indices to draw particles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

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
    cloudM.Init(size, gdl::Color::White);
    cloudC.Init(size, gdl::Color::Cyan);
    cloudY.Init(size, gdl::Color::Yellow);

    u64 deltaTimeStart = gettime();
    u64 programStart = gettime();
    float deltaTime = 0.0f;
    float mainElapsed = 0.0f;

    DeltaHistogram cpu = DeltaHistogram();

    while(true)
    {
        gdl::WiiInput::StartFrame();
        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            break;
        }
        VIDEO_WaitVSync();
    }

    // Update once to get all values set
    cloudM.Update(0.0f, Seek);
    cloudC.Update(0.0f, Seek);
    cloudY.Update(0.0f, Seek);
    ParticleMode modes[4] = {Loop, Rotate, SinWave, Seek};
    ParticleMode mode = modes[0];
    float modeCounter = 0.0f;


    bool gameRunning = true;
    while(true)
    {
        u64 now = gettime();
        deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
        deltaTimeStart = now;

        mainElapsed= (float)(now- programStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds

        modeCounter += deltaTime;
        if (modeCounter > 2.0f)
        {
            mode = (ParticleMode)(gdl::GetRandomInt(0,4));
            modeCounter -= 2.0f;
        }

        cpu.Update(gdl::Delta);
        gdl::WiiInput::StartFrame();

        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            gameRunning = false;
            // This starts the exit process
            gdl::wii::Exit();
        }


        if (gameRunning && false)
        {
            cloudM.Update(deltaTime, mode);
            // cloudC.Update(deltaTime);
            // cloudY.Update(deltaTime);
        }


        gdl::PrepDisplay();
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (gameRunning)
        {
            cloudM.Draw();
            // cloudC.Draw();
            //cloudY.Draw();

            // Test square
            glPushMatrix();
            glColor3f(1.0f, 0.0f, 1.0f);
            glRectf(-25.0f, -25.0f, 25.0f, 25.0f);
            glPopMatrix();

            if (false)
            {
            cpu.Draw(gdl::ScreenXres-256, 16, 64, gdl::Color::Yellow);
            // TODO How to draw the font?
            debugFont.Printf(gdl::ScreenXres-250, 18, 1.0f, gdl::Color::Black, "Delta");

            debugFont.Printf(10, 10, 1.0f, gdl::Color::Magenta, "Mode %u", (u32)mode);
            debugFont.Printf(10, 16, 1.0f, gdl::Color::Magenta, "mainElapsed %.2f", mainElapsed);
            }
        }


        gdl::Display();
    }
    cloudM.Quit();
    cloudC.Quit();
    cloudY.Quit();
}
