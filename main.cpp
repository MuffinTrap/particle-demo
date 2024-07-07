#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>

#include "mgdl-input-wii.h"
#include "particlecloud.h"
#include "quad.h"
#include "deltahistogram.h"
#include <GL/opengx.h>
#include <GL/glu.h>
#include "device.h"
#include <string>
#include "rocket_sync.h"

// ROCKET
//////////////////////////
static sync_device* rocket = nullptr;
static bool use_rocket = true;

void init()
{
    fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes, gdl::InitFlags::OpenGX);
    gdl::SetClearColor(gdl::Color::Black);
    gdl::WiiInput::Init();
    gdl::ConsoleMode();

    if (use_rocket)
    {
        std::string rocketHost = "192.168.10.52";//"localhost"
        rocket = ConnectRocket(rocketHost);
        if (rocket == nullptr)
        {
            use_rocket = false;
        }
    }
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
#include <math.h>
const int FLOWER_MAX_LEVEL = 10;
const double LEAVE_STEP = 0.3;

void drawLeaves(){
    glPushMatrix();
    GLfloat a = 1, b = 0.5;
    glTranslatef(0, 0, 0);
    glRotatef(30, 0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    //Leave height is  0.1 * cal.mysin(( abs(x)+ a) / a * 3.14) + 0.7
    for(double theta = 0 ; theta < 2 * 3.14 ; theta += LEAVE_STEP){
        GLfloat x = a * cosf(theta);
        GLfloat z = b * sinf(theta);
        GLfloat y = 0.1 * sinf((x+ a) / a * 3.14);
        glVertex3f(x, y, z);
    }
    glEnd();
    glPopMatrix();
}

void drawFlower(int flowerType, float centerLevel){
    //Base height is 1.4
    glPushMatrix();
    GLfloat a = 0.4, b = 0.1;//Control the shape of a flower
    if(flowerType == 1){
        b = 0.5;
    }
    //Just use top of first level of flower as its height.
    //and maximum tilt of first level is
    double alphaStep = 50.0 / centerLevel;      //Tilt degree of one level of flower
    unsigned int levelCnt = 0;
    //Flower part has the height of sin(60) * 2 * a
    //with a radius of 2 * a * cos(60)

    for(double alpha = 10.0; alpha < 60.0 && levelCnt < centerLevel; alpha += alphaStep, levelCnt++){
        GLfloat a0 = a * (60.0 - alpha) / 60.0 ;
        double phiStep = 360.0 / 9.0;     //Determined by number of pieces in a level
        phiStep = phiStep < 3 ? 3 : phiStep;
        for(double phi = 0 ; phi < 360 ; phi += phiStep) {      //One level of flowers
            glPushMatrix();
            glRotatef(phi, 0.0, 1.0, 0.0);
            glTranslatef(0.2, 0.0, 0.0);
            glRotatef(alpha, 0.0, 0.0, 1.0);        //Rotate by Z axis
            glTranslatef( a0, 0.0, 0.0);
            glBegin(GL_LINES);
            for(double theta = 0 ; theta < 2 * 3.14159f ; theta += LEAVE_STEP){
                GLfloat x = a0 * cosf(theta);
                GLfloat z = b * sinf(theta);
                GLfloat y = -0.05 * x * x;
                glVertex3f(x, y, z);
            }
            glEnd();
            glPopMatrix();
        }
    }
    glPopMatrix();
}
void drawAllFlower(float rootX, float rootY, float rootZ, float centerLevel){
    if(centerLevel==0) return;
    glPushMatrix();
    glTranslatef(rootX, rootY, rootZ);
    drawFlower(0, centerLevel);

    glColor3f(1, 1, 1);
    glPopMatrix();
}
int main()
{
    init();
    ogx_initialize();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_FLAT);
    glViewport(0, 0, gdl::ScreenXres, gdl::ScreenYres);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = (double)gdl::ScreenXres/(double)gdl::ScreenYres;


    // TODO How to set up the camera to look at particles???
    //glFrustum(aspect, aspect, -1.0, 1.0, 0.1f, 1000.0f);
    glOrtho(-40.0f * aspect, 40.0f * aspect, -40.0f, 40.0f, -0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Use arrays and indices to draw particles
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    gdl::ConsoleMode();


    ParticleCloud cloudM;
    ParticleCloud cloudC;
    ParticleCloud cloudY;
    int size = 4096;
    cloudM.Init(size, gdl::Color::White);
    cloudC.Init(size, gdl::Color::Cyan);
    cloudY.Init(size, gdl::Color::Yellow);

    u64 deltaTimeStart = gettime();
    u64 programStart = gettime();
    float deltaTime = 0.0f;
    float mainElapsed = 0.0f;

    DeltaHistogram cpu = DeltaHistogram();

    // Update once to get all values set
    cloudM.Update(0.0f, Seek);
    cloudC.Update(0.0f, Seek);
    cloudY.Update(0.0f, Seek);
    ParticleMode modes[4] = {Loop, Rotate, SinWave, Seek};
    ParticleMode mode = modes[0];
    float modeCounter = 0.0f;


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

    bool gameRunning = true;
    while(true)
    {
        u64 now = gettime();
        deltaTime = (float)(now - deltaTimeStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
        deltaTimeStart = now;

        mainElapsed= (float)(now- programStart) / (float)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds

        modeCounter += deltaTime;
        if (modeCounter > 2.0f)
        {
            mode = (ParticleMode)(gdl::GetRandomInt(0,4));
            modeCounter -= 2.0f;
        }

        cpu.Update(gdl::Delta);
        gdl::WiiInput::StartFrame();

        if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
            break;
        }

        // Update rocket sync
        if (use_rocket)
        {
            UpdateRocket(rocket);
        }


        if (gameRunning && false)
        {
            cloudM.Update(deltaTime, mode);
            // cloudC.Update(deltaTime);
            // cloudY.Update(deltaTime);
        }


        gdl::PrepDisplay();
        glClear(GL_COLOR_BUFFER_BIT);

        // Set the projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 640.0/480.0, 0.1, 100.0);

        if (gameRunning)
        {
            // Set the modelview matrix
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(sin(mainElapsed)*10.0f, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            cloudM.Draw();
            // cloudC.Draw();
            //cloudY.Draw();

            // Test square
            glPushMatrix();
            glColor3f(1.0f, 0.0f, 0.0f);
            glRectf(-2.0f, -2.0f, 2.0f, 2.0f);
            glPopMatrix();
        }


        gdl::Display();
    }
    cloudM.Quit();
    cloudC.Quit();
    cloudY.Quit();

    // Close rocket connection
    if (rocket != nullptr && use_rocket)
    {
        DisconnectRocket(rocket);
    }

    // Manual exit instead of gdl exit
    exit(0);
}
