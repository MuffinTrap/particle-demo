#pragma once
#if 0

#include "device.h"
#include <string>

sync_device* ConnectRocket(std::string hostIP);
void UpdateRocket(sync_device* device);
void DisconnectRocket(sync_device* device);

#endif
