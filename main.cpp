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
    const struct sync_track *clear_r;
    const struct sync_track *clear_g;
    const struct sync_track *clear_b;
    void keyboard(unsigned char key, int x, int y) {
        if (key == 27) { // ASCII code for 'Escape'
            // Clean up resources and exit the program
            free(data);
            alDeleteSources(1, &source);
            alDeleteBuffers(1, &buffer);
            alcMakeContextCurrent(NULL);
            alcDestroyContext(context);
            alcCloseDevice(device);
            sf_close(sndfile);
            start_save_sync("src/sync_data.h");
            save_sync(clear_r, "src/sync_data.h");
            save_sync(clear_g, "src/sync_data.h");
            save_sync(clear_b, "src/sync_data.h");

            //sync_destroy_device(rocket);
            exit(0); // Exit the program
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

void renderLoop() {
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
    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(-2.0f, -2.0f, 2.0f, 2.0f);
    glPopMatrix();
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
    printf("Setting up direction.");
    setupDirection();
    // Open the WAV file
    SF_INFO sfinfo;
    printf("Opening Music File: %s", filename);
    sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!sndfile) {
        printf("Error opening the file '%s'\n", filename);
        return 1;
    }
    printf("Setting up OpenAL Audio Device.");
    // Initialize OpenAL
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("Failed to open OpenAL device\n");
        sf_close(sndfile);
        return 1;
    }
    printf("Setting up OpenAL Audio Contex.");
    context = alcCreateContext(device, NULL);
    if (!context) {
        printf("Failed to create OpenAL context\n");
        alcCloseDevice(device);
        sf_close(sndfile);
        return 1;
    }

    rocket = sync_create_device("sync");
    if (!rocket) {
        printf("Out of memory?? GNU Rocket.");
        return 1;
    }
    clear_r = sync_get_track(rocket, "clear_r");
    clear_g = sync_get_track(rocket, "clear_g");
    clear_b = sync_get_track(rocket, "clear_b");

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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("My Mac/PC program window");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glutDisplayFunc(renderLoop);
    glutKeyboardFunc(keyboard); // Register the keyboard callback

    // Setup the timer callback
    glutTimerFunc(1000/60, timerFunc, 0);

    glutMainLoop();

    return 0;
}

 
int main(int argc, char** argv)
{
#ifdef GEKKO 
    init();
    ogx_initialize();
#else
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
