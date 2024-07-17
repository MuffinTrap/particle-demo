#include "debugcamera.h"
#include "crossOpenGL.h"
#include "crossVector3.h"

// Controls:
/*
 *  Nunchuck joystick (WASD):   Move on XZ plane
 *  D-pad (Arrow keys):         Up and Down : Move on Y-axis
                                Left and Right : Turn left and right
 *  Mouse: offset look direction

    B : (mouse 2)               Reset position to (0, 0, 1)
 *
 */


void DebugCamera::Init ( int scrW, int scrH )
{
    screenWidth = scrW;
    screenHeight = scrH;
	eye = glm::vec3(0.0f, 0.0f, 1.0f);
	dir = glm::vec3(0.0f, 0.0f, -1.0f);
	lookTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraSpeed = 2.4f;
	turnSpeedRadians = 0.8f;
}


glm::vec2 DebugCamera::NormalizedCursor(WiiController &controller)
{
    glm::vec2 cp = controller.GetCursorPosition();
    cp.x /= screenWidth;
    cp.x *= 2.0f;
    cp.x -= 1.0f;
    cp.y /= screenHeight;
    cp.y *= 2.0f;
    cp.y -= 1.0f;
    return cp;
}

void DebugCamera::Reset()
{
	eye = glm::vec3(0.0f, 0.0f, 1.0f);
	dir = glm::vec3(0.0f, 0.0f, -1.0f);
}

void DebugCamera::Update(float deltaTime, WiiController& controller)
{
    glm::vec2 joystick = controller.GetNunchukJoystickDirection(0.1f);
    bool left = controller.ButtonHeld(ButtonLeft);
    bool right =controller.ButtonHeld(ButtonRight);
    bool up = controller.ButtonHeld(ButtonUp);
    bool down =controller.ButtonHeld(ButtonDown);

    glm::vec3 rightVec;
    glm::vec3 upVec = WorldUp;
    rightVec = glm::cross(dir, upVec);
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

	normalizedCursor = NormalizedCursor(controller);
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
        glm::mat4 rotationMatrix(1);
        rotationMatrix = glm::rotate(rotationMatrix, turn, WorldUp);
        dir = glm::vec3(rotationMatrix * glm::vec4(dir, 1.0f));
    }

	if (controller.ButtonPress(ButtonB))
	{
		Reset();
	}
}

void DebugCamera::ApplyMatrix()
{
	// Set the modelview matrix
    glm::vec2 cp = normalizedCursor;
    lookTarget = eye + dir;
    glm::vec3 right;
    glm::vec3 up = WorldUp;
    right = glm::cross(dir, up);
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
