#include "wiicontroller.h"

WiiController::WiiController()
{
	channel = 0;
	ZeroAllInputs();
}


void WiiController::SetChannelNumber ( int channel )
{
	this->channel = channel;
}


bool WiiController::ButtonPress(int buttonEnum) {
  return (pressedButtons & buttonEnum) != 0;
}

bool WiiController::ButtonRelease(int buttonEnum) {
  return (releasedButtons & buttonEnum) != 0;
}

bool WiiController::ButtonHeld(int buttonEnum) {
  return (heldButtons & buttonEnum) != 0;
}

glm::vec2 WiiController::GetCursorPosition() {
  return glm::vec2(cursorX, cursorY);
}

glm::vec2 WiiController::GetNunchukJoystickDirection(float deadzone)
{
  nunchukJoystickDeadzone = deadzone;
  return glm::vec2(nunchukJoystickDirectionX, nunchukJoystickDirectionY);
}

float WiiController::GetRoll() {
  return roll;
}

void WiiController::ZeroAllInputs()
{
	pressedButtons = 0;
	releasedButtons = 0;
	heldButtons = 0;
	nunchukJoystickDirectionX = 0.0f;
	nunchukJoystickDirectionY = 0.0f;
	nunchukJoystickDeadzone = 0.0f;
	cursorX = 0.0f;
	cursorY = 0.0f;
	roll = 0.0f;
}

void WiiController::SetButtonDown ( int buttonEnum )
{
	pressedButtons += buttonEnum;
	heldButtons += buttonEnum;
}
void WiiController::SetButtonUp ( int buttonEnum )
{
	heldButtons -= buttonEnum;
}

