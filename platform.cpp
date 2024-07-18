#include "platform.h"


// Common includes
#include "crossOpenGL.h"
#include "demo.h"
#include "src/direction.hpp"

static Demo demoInstance;

// //////////////////////////////////
// NINTENDO WII
////////////////////////////////////
#ifdef GEKKO
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include <wiiuse/wpad.h>

static gdl::Sound demoMusic;
static WiiController controller;

static void ReadControllerInput ( WiiController& controllerInOut )
{
  WPAD_ScanPads();  // Scan the Wiimotes

  WPADData *data1 = WPAD_Data(controllerInOut.channel);

  const ir_t &ir = data1->ir;
  // Multiply x and y to match them to 16:9 screen
  controllerInOut.cursorX = ir.x * 1.67f - 16.f;
  controllerInOut.cursorY = ir.y * 1.2f - 16.f;

  controllerInOut.pressedButtons = WPAD_ButtonsDown(0);
  controllerInOut.releasedButtons = WPAD_ButtonsUp(0);
  controllerInOut.heldButtons = WPAD_ButtonsHeld(0);

  controllerInOut.nunchukJoystickDirectionX=0.0f;
  controllerInOut.nunchukJoystickDirectionY=0.0f;
  const expansion_t &ex = data1->exp;
  if (ex.type == WPAD_EXP_NUNCHUK)
  {
      joystick_t n = ex.nunchuk.js;
      if (n.mag > controllerInOut.nunchukJoystickDeadzone)
      {
          // Angle is reported in degrees
          // Angle of 0 means up.
          // 90 right, 180 down, 270 left

          float rad = DegToRad(n.ang);
          float x = 0;
          float y = -1.0f;
          float dirx = cos(rad) * x - sin(rad) * y;
          float diry = sin(rad) * x + cos(rad) * y;
          controllerInOut.nunchukJoystickDirectionX = dirx * n.mag;
          controllerInOut.nunchukJoystickDirectionY = diry * n.mag;
      }
  }

  controllerInOut.roll = DegToRad(data1->orient.roll);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Platform::Init ( int argc, char ** argv, ScreenAspect aspectRatio, bool useRocket )
{
    gdl::InitAspectMode mode = gdl::Aspect4x3;
    switch(aspectRatio)
    {
        case Screen4x3:
            mode = gdl::Aspect4x3;
            break;
        case Screen16x9:
            mode = gdl::Aspect16x9;
            break;
    };
	fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, mode, gdl::HiRes, gdl::InitFlags::OpenGX);
    ogx_initialize();

	// Init controller
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    controller.SetChannelNumber(0);

    gdl::ConsoleMode();
	demoInstance.Init(gdl::ScreenXres, gdl::ScreenYres, useRocket);


    while(true)
    {
        ReadControllerInput(controller);
        if (controller.ButtonPress(WPAD_BUTTON_HOME)) {
            break;
        }
        VIDEO_WaitVSync();
    }

}
#pragma GCC diagnostic pop

void Platform::LoadMusic(const char* filename, double beatsPerMin, int rowsPerBeat)
{
	setupDirection(beatsPerMin, rowsPerBeat);
	loadAudio(filename);
}

void Platform::PlayMusic()
{
	playAudio();
}

void Platform::RunMainLoop()
{
	float deltaTime;
	u64 now = gettime();
	u64 deltaTimeStart = now;
	PlayMusic();
	while (true)
	{
		// Timing
		u64 now = gettime();
		deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
		deltaTimeStart = now;

        ReadControllerInput(controller);
        updateAudio();

		if (controller.ButtonPress(WPAD_BUTTON_HOME)){
			break;
		}
        demoInstance.UpdateController(controller);
		demoInstance.Update(getTime(), deltaTime);
		gdl::PrepDisplay();

		demoInstance.Draw();

		glFlush();
		gdl::Display();
	}

	demoInstance.Quit();
	gdl::wii::DoProgramExit();
}



// ///////////////////////////////////
#else

// //////////////////////////////////
// WIN - MAC - LINUX
// //////////////////////////////////.

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sndfile.h>
#include <AL/al.h>

#include "glutInput.h"

static void quit()
{
	printf("Free resources\n");
	// Time to go
	// Game over time
		// Clean up resources and exit the program

    unloadAudio();
	demoInstance.Quit();

	exit(0);
}

void sceneUpdate() {

    WiiController* glutInput = GetGlutController();
    if (glutInput->ButtonPress(ButtonHome))
    {
		quit();
    }

    updateAudio();
    demoInstance.UpdateController(*glutInput);
	demoInstance.Update((float)getTime(), 0.016f);

    // Clear pressed buttons
    glutInput->pressedButtons = 0;

    // Update frame counter in direction.cpp
    frameCounterIncrement();

    // Tell glut that the window needs to be
    // redrawn.
    glutPostRedisplay();
}

void renderLoop() {

	demoInstance.Draw();

    // End drawing and process all commands
    glFlush();

    // Wait for v sync and swap
    glutSwapBuffers();
}


// ///////////////////////
// GLUT timer function
// calls the given function when 16 milliseconds have passed

#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void timerFunc(int value) {
    sceneUpdate();
    glutTimerFunc(1000/60, timerFunc, 0); // Re-register the timer callback
}
#pragma GCC diagnostic pop
#pragma clang diagnostic pop

static int scrw = 0;
static int scrh= 0;
void onWindowSizeChange(int newWidth, int newHeight)
{
    // Keep aspect ratio
    float width = (float)newWidth/(float)scrw;
    float height = (float)newHeight/(float)scrh;
    float scale = width < height ? width : height;

    float scaledWidth = scale*(float)scrw;
    float scaledHeight = scale*(float)scrh;

    int left = 0;
    int top = 0;

    if (scaledWidth < newWidth)
    {
        // Black bars on sides
        left = (newWidth - scaledWidth)/2;
    }
    if (scaledHeight < newHeight)
    {
        // Black bars on top and bottom
        top = (newHeight - scaledHeight)/2;
    }
    glViewport(left, top, (int)scaledWidth, (int)scaledHeight);
}

void Platform::Init ( int argc, char ** argv, ScreenAspect aspectRatio, bool useRocket )
{
    switch(aspectRatio)
    {
        case Screen4x3:
            screenWidth = 640;
            screenHeight = 480;
            break;
        case Screen16x9:
            screenWidth = 854;
            screenHeight = 480;
            break;
    };

    // Store these for the reshape callback
    scrw = screenWidth;
    scrh = screenHeight;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("My Mac/PC program window");

    glutDisplayFunc(renderLoop);

    // Input callbacks and init
    glutKeyboardFunc(keyboardDown); // Register the keyboard callback
    glutKeyboardUpFunc(keyboardUp); // Register the keyboard release

    glutSpecialFunc(specialKeyDown); // Register the keyboard callback
    glutSpecialUpFunc(specialKeyUp); // Register the keyboard release

    glutMouseFunc(mouseKey);        // Register mouse buttons and movement
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);
    GetGlutController()->ZeroAllInputs();

    glutReshapeFunc(onWindowSizeChange);

	demoInstance.Init(screenWidth, screenHeight, useRocket);
}

void Platform::RunMainLoop()
{
	PlayMusic();
    // Setup the timer callback
    glutTimerFunc(1000/60, timerFunc, 0);
    printf("Start glut main loop\n");
    glutMainLoop();
}

void Platform::LoadMusic(const char* filename, double beatsPerMin, int rowsPerBeat)
{
	setupDirection(beatsPerMin, rowsPerBeat);
	loadAudio(filename);
}

void Platform::PlayMusic()
{
    playAudio();
}

#endif // WIN - MAC - LINUX



