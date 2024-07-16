static bool gameRunning = true;

#define GL_SILENCE_DEPRECATION 1
#ifdef GEKKO
    #include <mgdl-wii.h>
    #include <wiiuse/wpad.h>
    #include <ogc/lwp_watchdog.h>

    #include "mgdl-input-wii.h"
#endif
#include <math.h>
#include "src/direction.hpp"
#include "rocket/sync.h"
#ifdef GEKKO 
    #include <GL/opengx.h>
    #include <GL/glu.h>
#else
    #ifdef __APPLE__
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <GLUT/glut.h>
    #else
        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glut.h>
    #endif

    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <sndfile.h>
    #include <AL/al.h>
    #include "rocket/track.h"
    struct sync_device *rocket;
    // Struct for rocket callbacsk
    static sync_cb rocket_callbacks;
    const struct sync_track *clear_r;
    const struct sync_track *clear_g;
    const struct sync_track *clear_b;
    static bool rocket_in_use = false; // Record if connection was succesfull
    void keyboard(unsigned char key, int x, int y) {
        if (key == 27) { // ASCII code for 'Escape'
            printf("exit pressed: save tracks\n");
            printf("Start save sync\n");
#ifndef SYNC_PLAYER

// #define SAVE_TO_HEADER
#ifdef SAVE_TO_HEADER
            // This tries to write to a header file, but on Linux it segfaults
            start_save_sync("src/sync_data.h");
            printf("save track clear_r\n");
            save_sync(clear_r, "src/sync_data.h");
            save_sync(clear_g, "src/sync_data.h");
            save_sync(clear_b, "src/sync_data.h");
#else
            // Save as binary file
            sync_save_tracks(rocket);
#endif

#endif// Syncplayer

            // This is noticed in the next sceneUpdate
            printf("Game running: false\n");
            gameRunning = false;
        }
    }
#endif

void glRectf(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);
        glVertex3f(x1, y1, -1.0f);
        glVertex3f(x2, y1, -1.0f);
        glVertex3f(x2, y2, -1.0f);
        glVertex3f(x1, y2, -1.0f);
    glEnd();
}

float mainElapsed = 0.0f;

// Scene update values
static float angle = 0.0f;

void sceneUpdate() {

    if (gameRunning == false)
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

        sync_destroy_device(rocket);

        // Dont draw anymore
        exit(0);
    }
    // Update music in direction.cpp
    updateAudio();
    // Here would update object positions etc.
    angle += 0.01f;


    // Update frame counter in direction.cpp
    frameCounterIncrement();

    // Tell glut that the window needs to be
    // redrawn.
    glutPostRedisplay();

}

void renderLoop() {

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
    glClear(GL_COLOR_BUFFER_BIT);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 640.0/480.0, 0.1, 100.0);

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(sin(mainElapsed)*10.0f, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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

    // End drawing and process all commands
    glFlush();

    // Wait for v sync and swap
    glutSwapBuffers();
}

#ifndef GEKKO 
void timerFunc(int value) {
    renderLoop();
    glutPostRedisplay();
    glutTimerFunc(1000/60, timerFunc, 0); // Re-register the timer callback
}
#endif


int initComputer(int argc, char** argv) {
    const char* filename = "test_music.wav"; // Replace with your WAV file path
    printf("Setting up direction.\n");
    setupDirection();
    // Open the WAV file
    SF_INFO sfinfo;
    printf("Opening Music File: %s\n", filename);
    sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!sndfile) {
        printf("Error opening the file '%s'\n", filename);
        return 1;
    }
    printf("Setting up OpenAL Audio Device.\n");
    // Initialize OpenAL
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("Failed to open OpenAL device\n");
        sf_close(sndfile);
        return 1;
    }
    printf("Setting up OpenAL Audio Contex.\n");
    context = alcCreateContext(device, NULL);
    if (!context) {
        printf("Failed to create OpenAL context\n");
        alcCloseDevice(device);
        sf_close(sndfile);
        return 1;
    }

    // Connect to rocket
    printf("Establishing rocket connection\n");
    rocket = sync_create_device("sync");
    if (!rocket) {
        printf("Out of memory?? GNU Rocket.");
        return 1;
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
        clear_r = sync_get_track(rocket, "clear_r");
        clear_g = sync_get_track(rocket, "clear_g");
        clear_b = sync_get_track(rocket, "clear_b");

        // Register callback functions to use with rocket
        rocket_callbacks =
        {
            // Defined in direction.cpp
            pause,
            set_row,
            is_playing
        };
    }
#endif

    printf("Load audio file\n");
    alcMakeContextCurrent(context);

    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    // Read audio data from the WAV file
    ALsizei dataSize = sfinfo.frames * sfinfo.channels * sizeof(short);
    data = (short*)malloc(dataSize);
    sf_read_short(sndfile, data, sfinfo.frames * sfinfo.channels);

    // Fill OpenAL buffer with audio data
    alBufferData(buffer, AL_FORMAT_STEREO16, data, dataSize, sfinfo.samplerate);

    // Set the source's buffer and play
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    printf("Init glut\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("My Mac/PC program window");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    printf("Register display and keyboard functions for glut\n");
    glutDisplayFunc(renderLoop);
    glutKeyboardFunc(keyboard); // Register the keyboard callback

    // Setup the timer callback
    glutTimerFunc(1000/60, timerFunc, 0);

    // Setup update functions
    glutIdleFunc(sceneUpdate);

    printf("Start glut main loop\n");
    glutMainLoop();


    return 0;
}

 
int main(int argc, char** argv)
{
#ifdef GEKKO 
    init();
    ogx_initialize();
#else
    printf("Main start\n");
    initComputer(argc,argv);
#endif
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_FLAT);
#ifdef GEKKO 
    glViewport(0, 0, gdl::ScreenXres, gdl::ScreenYres);
#else
    glViewport(0, 0, 640, 480);
#endif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

#ifdef GEKKO 
    double aspect = (double)gdl::ScreenXres/(double)gdl::ScreenYres;
#else
    double aspect = 4.0/3.0;
#endif

    // TODO How to set up the camera to look at particles???
    //glFrustum(aspect, aspect, -1.0, 1.0, 0.1f, 1000.0f);
    glOrtho(-40.0f * aspect, 40.0f * aspect, -40.0f, 40.0f, -0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Use arrays and indices to draw particles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

#ifdef GEKKO 
    gdl::ConsoleMode();
#endif

    // Init your Game here
    /*
             Uncomment to see console messages
             before game starts
    */

#ifdef GEKKO 
    unsigned long deltaTimeStart = gettime();
    unsigned long programStart = gettime();
#endif
    float deltaTime = 0.0f;

    float modeCounter = 0.0f;

    bool gameRunning = true;
    while(true)
    {
#ifdef GEKKO 
        unsigned long now = gettime();
        
        deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
        
        deltaTimeStart = now;

        mainElapsed= (float)(now- programStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
#endif

        modeCounter += deltaTime;

#ifdef GEKKO 
        cpu.Update(gdl::Delta);
        gdl::WiiInput::StartFrame();

        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            gameRunning = false;
            // This starts the exit process
            gdl::wii::Exit();
        }

        gdl::PrepDisplay();
#endif
        if (gameRunning)
        {
            renderLoop();
        }
#ifdef GEKKO 
        gdl::Display();
#endif
    }
}
