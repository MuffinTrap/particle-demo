#include "direction.hpp"

// ////////////////////////////////////
//            COMMON

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

static const bool music_enabled = true;
static double __attribute__((__unused__)) time_secs = 0.0;
static double __attribute__((__unused__)) time_secs_offset = 0.0f;
static int time_frames = 0;
double bpm; /* beats per minute */
int rpb; /* rows per beat */
double row_rate;


// //////////////////////////////////////
//           NINTENDO WII

#ifdef GEKKO
    //TODO: 

#else

///////////////////////////////////////////
// WIN - MAC - LINUX

    #include <sndfile.h>
    #include <AL/al.h>
    #include <AL/alc.h>

/***AUDIO PUBLIC***/

ALuint buffer, source;
ALCcontext* context;
SNDFILE* sndfile;
short* data;
ALCdevice* device;
SF_INFO sfinfo;

int loadAudio(const char* filename)
{
    // Open the WAV file
    SF_INFO sfinfo;
    printf("Opening Music File: %s\n", filename);
    sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!sndfile) {
        printf("Error opening the file '%s'\n", filename);
        return 1;
    }
    printf("Setting up OpenAL Audio Device.\n");
    // Initialize OpenAL
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("Failed to open OpenAL device\n");
        sf_close(sndfile);
        return 1;
    }
    printf("Setting up OpenAL Audio Contex.\n");
    context = alcCreateContext(device, NULL);
    if (!context) {
        printf("Failed to create OpenAL context\n");
        alcCloseDevice(device);
        sf_close(sndfile);
        return 1;
    }

    printf("Load audio file\n");
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
    return 0;
}

void playAudio()
{
    alSourcePlay(source);
}

// Returns the play position in seconds
void updateAudio()
{
	ALint sampleOffset;
	alGetSourcei(source, AL_SAMPLE_OFFSET, &sampleOffset);

	// Assuming you've already opened the audio file using sf_open
	sf_command(sndfile, SFC_GET_LOG_INFO, &sfinfo, sizeof(sfinfo));
	double sampleRate = 44100.0;
	time_secs = (double)sampleOffset / sampleRate;
}

void setupDirection() {
	bpm = 162.0; /* beats per minute */
	rpb = 8; /* rows per beat */
	row_rate = ((double)(bpm) / 60) * rpb;
}

double getTime() {
	return time_secs;
}

bool isMusicEnabled() {
	return music_enabled;
}

void frameCounterIncrement() {
	time_frames++;
}

double get_row() 
{
	return time_secs * row_rate + 0.016;
}

// These are the rocket sync callback functions
void pause( int __attribute__((__unused__)) flag)
{
	if (flag)
    	alSourcePause(source);
	else
    	alSourcePlay(source);
}

void set_row( int __attribute__((__unused__)) row)
{
    double frow = row - 0.01;
	alSourcef(source, AL_SEC_OFFSET, frow / row_rate);
}

int is_playing() 
{
    // Get the play state of the audio source
    ALint sourceState;
    alGetSourcei(source, AL_SOURCE_STATE, &sourceState);

    if (sourceState == AL_PLAYING) {
        return 1;
    } else if (sourceState == AL_PAUSED) {
        return 0;
    } else if (sourceState == AL_STOPPED) {
        return 0;
    } else {
        return 2;
    }
}

#endif // WIN - MAC - LINUX
