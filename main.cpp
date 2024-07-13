#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include <string>
#include "mgdl-input-wii.h"

// OpenGx
#include <GL/opengx.h>
#include <GL/glu.h>

//
#include "palette.h"


// Debug
#include "deltahistogram.h"
#include "debugcamera.h"

// ROCKET
//////////////////////////
#include "rocket_sync.h"
#include "device.h"
static sync_device* rocket = nullptr;
static bool use_rocket = false;

void init()
{
    fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes, gdl::InitFlags::OpenGX);
    gdl::SetClearColor(gdl::Color::Black);
    gdl::WiiInput::Init();
    gdl::ConsoleMode();

    if (use_rocket)
    {
        // Replace with host computer IP address
        std::string rocketHost = "localhost";
        rocket = ConnectRocket(rocketHost);
        if (rocket == nullptr)
        {
            use_rocket = false;
        }
    }
}

// Dont complain about mainElapsed etc..
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

int main()
{
    init();
    ogx_initialize();
    PaletteClearColor3f(BLACK);
    glShadeModel(GL_FLAT);
    glCullFace(GL_BACK);
    glViewport(0, 0, gdl::ScreenXres, gdl::ScreenYres);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)gdl::ScreenXres/(float)gdl::ScreenYres;
    gluPerspective(45.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Use arrays and indices to draw particles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
	glEnable(GL_TEXTURE_2D);

    gdl::ConsoleMode();

    DebugCamera camera;

    u64 deltaTimeStart = gettime();
    u64 programStart = gettime();
    float deltaTime = 0.0f;
    float mainElapsed = 0.0f;


    // Uncomment this block to see console messages before game starts
    /*
    while(true)
    {
        gdl::WiiInput::StartFrame();
        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)) {
            break;
        }
        VIDEO_WaitVSync();
    }
    */

    while(true)
    {
        u64 now = gettime();
        deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
        deltaTimeStart = now;

        mainElapsed= (float)(now- programStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds

        gdl::WiiInput::StartFrame();

        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            // Break the game loop.
            break;
        }


        // Update rocket sync
        if (use_rocket)
        {
            UpdateRocket(rocket);
        }

        camera.Update(deltaTime);

        // Start drawing
        gdl::PrepDisplay();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set the projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspect, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        camera.ApplyMatrix(); // Calls gluLookAt()

        /*
        glPushMatrix();
            camera.Draw3DAxii();
        glPopMatrix();
        */

        glFlush();
        gdl::Display();
    }

    // Close rocket connection
    if (rocket != nullptr && use_rocket)
    {
        DisconnectRocket(rocket);
    }

    // Manual exit instead of gdl exit
    exit(0);
}
#pragma GCC diagnostic pop
