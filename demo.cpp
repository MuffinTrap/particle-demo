#include "demo.h"
#include "crossOpenGL.h"

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
        rocket_callbacks.pause = rocket_pause;
        rocket_callbacks.is_playing = rocket_is_playing;
        rocket_callbacks.set_row = rocket_set_row;
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

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    PaletteClearColor3f(BLACK);
    glShadeModel(GL_SMOOTH);

    // On Wii the winding is ClockWise and cannot be changed
#ifndef GEKKO
    glFrontFace(GL_CW);
#endif

    // Face culling
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

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

    host.Update();
}

// Draw effects using OpenGL functions
void Demo::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, aspect, 0.1, 100.0);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 1.0,
              0.0, 0.0, 0.0,  // Center
              0.0, 1.0, 0.0); // Up

    host.Draw();
}

bool Demo::QuitRequested()
{
    return host.activeEffect == fxQUIT_DEMO;
}

// Release memory
void Demo::Save()
{
	if (rocket_in_use)
	{
#ifndef SYNC_PLAYER

    #define SAVE_TO_HEADER
    #ifdef SAVE_TO_HEADER
                printf("Saving tracks to header file\n");
                start_save_sync(SYNC_FILE_H, SYNC_FILE_CPP);

                host.Save();

                end_save_sync(SYNC_FILE_H, SYNC_FILE_CPP);
    #else
                // Save as binary file:
                // The Wii exe fails to read these, maybe endianness error?
                sync_save_tracks(rocket);
    #endif // Save to header

#endif// Syncplayer
	}
}

void Demo::Free()
{
    if (rocket_in_use)
    {
        sync_destroy_device(rocket);
    }
    host.Free();
}


