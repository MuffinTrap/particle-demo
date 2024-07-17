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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Platform::Init ( int argc, char ** argv, bool useRocket )
{
	fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes, gdl::InitFlags::OpenGX);

	// Init controller
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    gdl::ConsoleMode();
	demoInstance.Init(gdl::ScreenXres, gdl::ScreenYres, useRocket);

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
    WiiController ctrl0;
    ctrl0.SetChannelNumber(0);
	PlayMusic();
	while (ESC_PRESSED == false)
	{
		// Timing
		u64 now = gettime();
		deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
		deltaTimeStart = now;

        ReadControllerInput(ctrl0);

		if (ctrl0.ButtonPress(WPAD_BUTTON_HOME)){
			ESC_PRESSED = true;
		}
		demoInstance.Update(0.0f, deltaTime);
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
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

void Platform::Init ( int argc, char ** argv, bool useRocket )
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("My Mac/PC program window");

    glutDisplayFunc(renderLoop);
    glutKeyboardFunc(keyboard); // Register the keyboard callback
	glutIdleFunc(sceneUpdate);

	demoInstance.Init(640, 480, useRocket);
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



