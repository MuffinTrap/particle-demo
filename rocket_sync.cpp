#include "rocket_sync.h"

#include <math.h>

#if 0

static float sync_row = 0.0f;
static bool music_playing = false;
std::string hostAddress = "localhost";

// Functions for row etc


// Functions needed by rocket update
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void music_pause(void *d, int flag)
{
    if (flag)
    {
        // Pause music
        music_playing = false;

    }
    else
    {
        // Play music
        music_playing = true;
    }
}

static void music_set_row(void *d, int row)
{
    // Set music playback position to given row
    sync_row = row;
}

static int music_is_playing(void *d)
{
    // return 1 if music is playing
    // return 0 if not
    if (music_playing) return 1;
    else return 0;
}
#pragma GCC diagnostic pop

static struct sync_cb sync_functions = {
    music_pause,
    music_set_row,
    music_is_playing
};


// Creates a rocket sync device.
// Returns nullptr if something goes wrong
sync_device * ConnectRocket ( std::string hostIP )
{
	// Set up networking
	bool use_dhcp = true;
	int retries = 10;

	// As per devkitpro/examples/devices/sockettest, these
	// are zero and the if_config fills them in
	char ip[16] = {0};
	char mask[16] = {0};
	char gate[16] = {0};
	s32 if_config_status = if_config(ip, mask, gate, use_dhcp, retries);
	if (if_config_status >= 0)
	{
		printf("Network configured: ip: %s, gw: %s, mask: %s\n", ip, gate, mask);
	}
	else
	{
		printf("Network configuration failed\n");
		return nullptr;
	}

	// Init and connect Rocket
	sync_device* rocket = sync_create_device("wii");
	if (rocket == nullptr)
	{
		printf( "Failed to create rocket sync device\n");
		return nullptr;
	}

	// Default port is 1338
	int connectResult = sync_tcp_connect(rocket, hostIP.c_str(), SYNC_DEFAULT_PORT);
	if(connectResult == 0)
	{
		printf("Rocket connected\n");
		hostAddress = hostIP;
		return rocket;
	}
	else if (connectResult == SYNC_DEVICE_ERROR_CONNECTION_FAILED)
	{
		printf("Rocket connection failed\n");
	}
	else if (connectResult == SYNC_DEVICE_ERROR_TRACK_FETCH_FAIL)
	{
		printf("Rocket failed to fetch tracks\n");
	}
	else
	{
		printf("Rocket connection failed for unknown reason\n");
	}
	return nullptr;
}

void UpdateRocket ( sync_device* device )
{
	if (sync_update(device, (int)floor(sync_row), &sync_functions, nullptr))
	{
		// Reconnect
		sync_tcp_connect(device, hostAddress.c_str(), SYNC_DEFAULT_PORT);
	}
}

void DisconnectRocket ( sync_device* device )
{
	sync_save_tracks(device);
	sync_destroy_device(device);
}




#endif
