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
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include "mgdl-input-wii.h"

static gdl::Sound demoMusic;

void Platform::Init ( int argc, char ** argv, bool useRocket )
{

    gdl::init();
    ogx_initialize();
    gdl::ConsoleMode();

	demoInstance.Init(gdl::ScreenXres, gdl::ScreenYres, useRocket);
}

void Platform::LoadMusic(const char* filename)
{
	demoMusic.LoadFile(filename);
}

void Platform::PlayMusic()
{
	demoMusic.Play();
}

void Platform::RunMainLoop()
{
	PlayMusic();
	while (true)
	{
		gdl::WiiInput::StartFrame();

		if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
			break;
		}
		demoInstance.Update(0.0f, 0.0f);
		gdl::PrepDisplay();

		demoInstance.Draw();

		gdl::Display();
	}

	demoInstance.Quit();
	gdl::DoProgramExit();
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


void Platform::LoadMusic(const char* filename)
{
	loadAudio(filename);
}

void Platform::PlayMusic()
{
    playAudio();
}

#endif // WIN - MAC - LINUX



