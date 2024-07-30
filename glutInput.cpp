
#ifndef GEKKO
#include "glutInput.h"
#include "crossOpenGL.h"

static WiiController glutController;

#define ascii_1  49
#define ascii_2  50
#define ascii_w  119
#define ascii_a  97
#define ascii_s  115
#define ascii_d  100
#define ascii_q  113
#define ascii_e  101

#define ascii_ESC 27

WiiController* GetGlutController()
{
	return &glutController;
}

#pragma GCC diagnostic push
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-parameter"

void mouseKey(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			glutController.SetButtonDown(ButtonA);
		}
		else {
			glutController.SetButtonUp(ButtonA);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN) {
			glutController.SetButtonDown(ButtonB);
		}
		else {
			glutController.SetButtonUp(ButtonB);
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON) {
		// Nop
	}
}

void mouseMove(int x, int y) {
	glutController.cursorX = x;
	glutController.cursorY = y;
}

void keyboardDown(unsigned char key, int x, int y) {
	if (key == ascii_ESC) { // ASCII code for 'Escape'
		glutController.SetButtonDown(ButtonHome);
	}
	else if (key == ascii_w) {
		glutController.nunchukJoystickDirectionY = -1.0f;
	}
	else if (key == ascii_s) {
		glutController.nunchukJoystickDirectionY = 1.0f;
	}
	else if (key == ascii_a) {
		glutController.nunchukJoystickDirectionX = -1.0f;
	}
	else if (key == ascii_d) {
		glutController.nunchukJoystickDirectionX = 1.0f;
	}
	else if (key == ascii_1) {
		glutController.SetButtonDown(Button1);
	}
	else if (key == ascii_2) {
		glutController.SetButtonDown(Button2);
	}
	else if (key == ascii_q) {
		glutController.SetButtonDown(ButtonMinus);
	}
	else if (key == ascii_e) {
		glutController.SetButtonDown(ButtonPlus);
	}
}

void keyboardUp(unsigned char key, int x, int y) {
	if (key == ascii_ESC) { // ASCII code for 'Escape'
		glutController.SetButtonUp(ButtonHome);
	}
	else if (key == ascii_w) {
		glutController.nunchukJoystickDirectionY = 0.0f;
	}
	else if (key == ascii_s) {
		glutController.nunchukJoystickDirectionY = 0.0f;
	}
	else if (key == ascii_a) {
		glutController.nunchukJoystickDirectionX = 0.0f;
	}
	else if (key == ascii_d) {
		glutController.nunchukJoystickDirectionX = 0.0f;
	}
	else if (key == ascii_1) {
		glutController.SetButtonUp(Button1);
	}
	else if (key == ascii_2) {
		glutController.SetButtonUp(Button2);
	}
	else if (key == ascii_q) {
		glutController.SetButtonUp(ButtonMinus);
	}
	else if (key == ascii_e) {
		glutController.SetButtonUp(ButtonPlus);
	}
}

void specialKeyDown(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
		glutController.SetButtonDown(ButtonUp);
	}
    else if (key == GLUT_KEY_DOWN) {
		glutController.SetButtonDown(ButtonDown);
	}
    else if (key == GLUT_KEY_LEFT) {
		glutController.SetButtonDown(ButtonLeft);
	}
    else if (key == GLUT_KEY_RIGHT) {
		glutController.SetButtonDown(ButtonRight);
	}
	else if (key == GLUT_KEY_F5) {
		glutController.SetButtonDown(ButtonPlus);
	}
}

void specialKeyUp(int key, int x, int y) {

    if (key == GLUT_KEY_UP) {
		glutController.SetButtonUp(ButtonUp);
	}
    else if (key == GLUT_KEY_DOWN) {
		glutController.SetButtonUp(ButtonDown);
	}
    else if (key == GLUT_KEY_LEFT) {
		glutController.SetButtonUp(ButtonLeft);
	}
    else if (key == GLUT_KEY_RIGHT) {
		glutController.SetButtonUp(ButtonRight);
	}
	else if (key == GLUT_KEY_F5) {
		glutController.SetButtonUp(ButtonPlus);
	}
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

#endif
