#include "debugcamera.h"
#include "mgdl-input-wii.h"
#include <wiiuse/wpad.h>
#include <GL/opengx.h>
#include <GL/glu.h>


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
	float s = cameraSpeed * deltaTime;
    if (gdl::WiiInput::ButtonHeld(WPAD_BUTTON_RIGHT))
	{
        eye.x += s;
	}
    if (gdl::WiiInput::ButtonHeld(WPAD_BUTTON_LEFT))
	{
        eye.x -= s;
	}
    if (gdl::WiiInput::ButtonHeld(WPAD_BUTTON_UP))
	{
        eye.y += s;
	}
    if (gdl::WiiInput::ButtonHeld(WPAD_BUTTON_DOWN))
	{
        eye.y -= s;
	}

    if (gdl::WiiInput::ButtonHeld(WPAD_BUTTON_PLUS))
	{
        eye.z -= s;
	}
    if (gdl::WiiInput::ButtonHeld(WPAD_BUTTON_MINUS))
	{
        eye.z += s;
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
	gluLookAt(eye.x, eye.y, eye.z,
				0.0 + cp.x, 0.0 + cp.y, 0.0,  // Center
				0.0, 1.0, 0.0); // UP
}
