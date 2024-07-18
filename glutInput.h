#pragma once
#ifndef GEKKO
#include "wiicontroller.h"
// Functions for listening to keyboard and mouse
// using GLUT
extern "C" {

WiiController* GetGlutController();
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void specialKeyUp(int key, int x, int y);
void specialKeyDown(int key, int x, int y);
void mouseKey(int button, int state, int x, int y);
void mouseMove(int x, int y);

}

#endif
