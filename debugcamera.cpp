#include "debugcamera.h"
#include "mgdl-input-wii.h"
#include <wiiuse/wpad.h>
#include <GL/opengx.h>
#include <GL/glu.h>
#include <mgdl-wii.h>

// Controls:
/*
 *  Nunchuck joystick (WASD):   Move on XZ plane
 *  D-pad (Arrow keys):         Up and Down : Move on Y-axis
                                Left and Right : Turn left and right
 *  Mouse: offset look direction

    B : (mouse 2)               Reset position to (0, 0, 1)
 *
 */



static gdl::vec2 NormalizedCursor()
{
    gdl::vec2 cp = gdl::WiiInput::GetCursorPosition();
    cp.x /= gdl::ScreenXres;
    cp.x *= 2.0f;
    cp.x -= 1.0f;
    cp.y /= gdl::ScreenYres;
    cp.y *= 2.0f;
    cp.y -= 1.0f;
    return cp;
}

void DebugCamera::Reset()
{
	eye = {0.0f, 0.0f, 1.0f};
}

void DebugCamera::Update(float deltaTime)
{
    gdl::vec2 joystick = gdl::WiiInput::GetNunchukJoystickDirection(0.1f);
    bool left = gdl::WiiInput::ButtonHeld(WPAD_BUTTON_LEFT);
    bool right =gdl::WiiInput::ButtonHeld(WPAD_BUTTON_RIGHT);
    bool up = gdl::WiiInput::ButtonHeld(WPAD_BUTTON_UP);
    bool down =gdl::WiiInput::ButtonHeld(WPAD_BUTTON_DOWN);

    guVector rightVec;
    guVector upVec = gdl::UP;
    guVecCross(&dir, &upVec, &rightVec);
    eye = eye + (rightVec * joystick.x) * cameraSpeed * deltaTime;
    eye = eye - (dir * joystick.y) * cameraSpeed * deltaTime;

    if (up)
    {
        eye.y += cameraSpeed * deltaTime;
    }
    if (down)
    {
        eye.y -= cameraSpeed * deltaTime;
    }

    float turn = 0.0f;
    if (left)
    {
        turn = turnSpeedRadians * deltaTime;
    }
    else if (right)
    {
        turn = -turnSpeedRadians * deltaTime;
    }
    if (left || right)
    {
        Mtx rot;
        guVector up = gdl::UP;
        guMtxRotAxisRad(rot, &up, turn);
        guVecMultiply(rot, &dir, &dir);
    }

	if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_B))
	{
		Reset();
	}
}

void DebugCamera::ApplyMatrix()
{
	// Set the modelview matrix
	gdl::vec2 cp = NormalizedCursor();
    lookTarget = eye + dir;
    guVector right;
    guVector up = gdl::UP;
    guVecCross(&dir, &up, &right);
	gluLookAt(eye.x, eye.y, eye.z,
				lookTarget.x + (right * cp.x).x,
              lookTarget.y + (up * cp.y).y,
              lookTarget.z,  // Center
				0.0, 1.0, 0.0); // UP
}

void DebugCamera::Draw3DAxii()
{
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.2f, 0.2f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);

        glColor3f(0.2f, 1.0f, 0.2f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        glColor3f(0.2f, 0.2f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}
