#include "platform.h"


// Common includes
#include "crossOpenGL.h"
#include "demo.h"
#include "src/direction.hpp"

static Demo demoInstance;
static bool ESC_PRESSED = false;

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
	while (ESC_PRESSED == false)
	{
		// Timing
		u64 now = gettime();
		deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
		deltaTimeStart = now;

        ReadControllerInput(controller);
        updateAudio();

		if (controller.ButtonPress(WPAD_BUTTON_HOME)){
			ESC_PRESSED = true;
		}
		demoInstance.Update(getTime(), deltaTime);
		gdl::PrepDisplay();

		demoInstance.Draw();

		glFlush();
		gdl::Display();
	}

	demoInstance.Quit();
	gdl::wii::DoProgramExit();
}

void Platform::ReadControllerInput ( WiiController& controllerInOut )
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

static void quit()
{
	printf("Free resources\n");
	// Time to go
	// Game over time
		// Clean up resources and exit the program
	alSourceStop(source);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	sf_close(sndfile);

	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	free(data);

	demoInstance.Quit();

	// Dont draw anymore
	exit(0);

}

void sceneUpdate() {

    if (ESC_PRESSED)
    {
		quit();
    }

    updateAudio();
	demoInstance.Update((float)getTime(), 0.016f);

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

#pragma GCC diagnostic push
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void keyboard(unsigned char key, int x, int y) {
        if (key == 27) { // ASCII code for 'Escape'
            printf("exit pressed: save tracks\n");
            printf("Start save sync\n");

            // This is noticed in the next sceneUpdate
            printf("Game running: false\n");
            ESC_PRESSED = true;
        }
    }
// ///////////////////////
// GLUT timer function
// calls the given function when 16 milliseconds have passed

void timerFunc(int value) {
    sceneUpdate();
    glutTimerFunc(1000/60, timerFunc, 0); // Re-register the timer callback
}

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
    // Don't do anything?

}
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

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
    scrw = screenWidth;
    scrh = screenHeight;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("My Mac/PC program window");

    glutDisplayFunc(renderLoop);
    glutKeyboardFunc(keyboard); // Register the keyboard callback
	glutIdleFunc(sceneUpdate);
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

void Platform::ReadControllerInput ( WiiController& controllerInOut )
{
	controllerInOut.ZeroAllInputs();
}

#endif // WIN - MAC - LINUX



