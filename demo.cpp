#include "demo.h"
#include "crossOpenGL.h"
#include <math.h>

#include "palette.h"

// Rocket sync includes
#include "src/direction.hpp"
#include "rocket/sync.h"
#include "rocket/track.h"

// Rocket sync variables
struct sync_device *rocket;

// Struct for rocket callbacsk
#ifndef SYNC_PLAYER
    // This is for the win-mac-linux version
    // Listen to the editor and store tracks through pointers
    static sync_cb rocket_callbacks;
#else

    // This is for the wii version.
    // Read track data from a header
    // This header contains the sync_tracks
    #include "src/sync_data.h"
#endif


bool Demo::ConnectRocket()
{
    // Connect to rocket
    printf("Establishing rocket connection\n");
    rocket = sync_create_device("sync");
    if (!rocket) {
        printf("Out of memory?? GNU Rocket.");
        return false;
    }

#ifndef SYNC_PLAYER
    // Connect rocket editor
    if (sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT))
    {
        printf("Failed to connect to rocket\n");
        rocket_in_use = false;
    }
    else
    {
        printf("Rocket connected\n");
        rocket_in_use = true;

        // Register callback functions defined in direction.cpp to use with rocket
        rocket_callbacks.pause = pause;
        rocket_callbacks.is_playing = is_playing;
        rocket_callbacks.set_row = set_row;
    }

#endif

	return true;
}

// Create effects, reserve memory
void Demo::Init(int scrW, int scrH, bool useRocket)
{
	screenWidth = scrW;
	screenHeight = scrH;
	aspect = (float)screenWidth/(float)screenHeight;

    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    PaletteClearColor3f(BLACK);
    glShadeModel(GL_FLAT);

    // Use arrays and indices to draw particles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    rocket_in_use = useRocket;
	if (useRocket)
	{
		ConnectRocket();
	}

	camera.Init(screenWidth, screenHeight);
	host.Init(rocket);
}

void Demo::UpdateController(WiiController& controllerState)
{
    controller = controllerState;
}

void Demo::Update ()
{
    // Read track values from rocket
    #ifndef SYNC_PLAYER
    if (rocket_in_use)
    {
        if (sync_update(rocket, (int)floor(get_row()), &rocket_callbacks))
        {
            // Reconnect if sync fails
            sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT);
        }
    }
    #endif

    if (controller.ButtonPress(Button1))
    {
        host.activeEffect -= 1.0f;
    }
    if (controller.ButtonPress(Button2))
    {
        host.activeEffect += 1.0f;
    }
    host.Update();
}

// Draw effects using OpenGL functions
void Demo::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, aspect, 0.1, 100.0);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z,
              0.0, camera.lookTarget.y, 0.0,  // Center
              0.0, 1.0, 0.0); // Up

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -1.0f);
        host.Draw();
    glPopMatrix();
}

// Release memory
void Demo::Quit()
{
	if (rocket_in_use)
	{
#ifndef SYNC_PLAYER

#define SAVE_TO_HEADER
#ifdef SAVE_TO_HEADER
            printf("Saving tracks to header file\n");
            start_save_sync("src/sync_data.h");

            host.Quit();
#else
            // Save as binary file:
            // The Wii exe fails to read these, maybe endianness error?
            sync_save_tracks(rocket);
#endif

#endif// Syncplayer
        sync_destroy_device(rocket);
	}
}


