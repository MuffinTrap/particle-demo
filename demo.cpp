#include "demo.h"
#include "crossOpenGL.h"
#include <math.h>

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
const struct sync_track *clear_r;
const struct sync_track *clear_g;
const struct sync_track *clear_b;

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

    // Get the pointers to the tracks in the editor
    clear_r = sync_get_track(rocket, "clear_r");
    clear_g = sync_get_track(rocket, "clear_g");
    clear_b = sync_get_track(rocket, "clear_b");
#endif

    // test reading track
    float first_red = sync_get_val(clear_r, 0.0);
    printf("Red on first row is %.2f\n", first_red);

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

    glClearColor(0.1f, 0.1f, 0.7f, 0.0f);
    glShadeModel(GL_FLAT);

    // Use arrays and indices to draw particles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    rocket_in_use = useRocket;
	if (useRocket)
	{
		ConnectRocket();
	}
}

// elapsed : for effects that are in sync with music
// deltaTime : for effects that run constantly and are not in sync with music
void Demo::Update ( float elapsed, float deltaTime )
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

	angle = elapsed * deltaTime * 360.0f;

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

// Draw effects using OpenGL functions
void Demo::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 100.0);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0, 10.0, // Eye
              0.0, 0.0, 0.0,  // Center
              0.0, 1.0, 0.0); // Up
    // Test square
    glPushMatrix();
    if (rocket_in_use)
    {
        glColor3f(
            (float)(sync_get_val(clear_r, get_row())),
            (float)(sync_get_val(clear_g, get_row())),
            (float)(sync_get_val(clear_b, get_row()))
        );
    }
    else
    {
        glColor3f(1.0f, 1.0f, 0.0f);
    }

    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glRectf(-2.0f, -2.0f, 2.0f, 2.0f);
    glPopMatrix();

    glPushMatrix();
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.6f, 0.6f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, 0.0f);

    glEnd();


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
            start_save_sync("src/sync_data.h");
            printf("save track clear_r\n");
            save_sync(clear_r, "src/sync_data.h");
            save_sync(clear_g, "src/sync_data.h");
            save_sync(clear_b, "src/sync_data.h");
#else
            // Save as binary file:
            // The Wii exe fails to read these, maybe endianness error?
            sync_save_tracks(rocket);
#endif

#endif// Syncplayer
        sync_destroy_device(rocket);
	}
}


