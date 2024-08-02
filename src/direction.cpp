#include "direction.hpp"

// ////////////////////////////////////
//            COMMON

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include "../crossAssert.h"

static const bool music_enabled = true;
static double __attribute__((__unused__)) time_secs = 0.0;
static double __attribute__((__unused__)) time_secs_offset = 0.0f;
static int time_frames = 0;
static double bpm; /* beats per minute */
static int rpb; /* rows per beat */
static double row_rate;
static int sample_rate = 0; // Read this from the file

void setupDirection(double beatsPerMin, int rowsPerBeat) {
	bpm = beatsPerMin;
	rpb = rowsPerBeat;
	row_rate = (bpm / 60.0) * (double)rpb;
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
    // Without ceil() will return row 0 twice
    // and then by off by one
	return ceil(time_secs * row_rate);
}


#ifdef GEKKO
// //////////////////////////////////////
//           NINTENDO WII

gdl::Sound musicSound;

int loadAudio(const char* filename)
{
    bool loadOk = musicSound.LoadSound(filename);
    gdl_assert(loadOk, "Failed to load Wii music");
    if (loadOk) {
        return 0;
    }
    else {
        return 1;
    }
}

void updateAudio()
{
    time_secs = musicSound.GetElapsedSeconds();
}

void playAudio()
{
    bool started = musicSound.Play(1.0f, 100.0f);
    gdl_assert_print(started, "Music no start!");
}

void unloadAudio()
{
    musicSound.Delete();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void rocket_set_row(int row)
{
    // * NOT IMPLEMENTED ON WII *//
}
#pragma GCC diagnostic pop

void pause(int flag)
{
    musicSound.Pause(flag == 1 ? true : false);
}

int is_playing()
{
    return (musicSound.GetIsPlaying() ? 1 : 0);
}

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

    printf("Load audio file. Sample rate %d\n", sfinfo.samplerate);
    sample_rate = sfinfo.samplerate;

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

    sf_close(sndfile);
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
	time_secs = (double)sampleOffset / (double)sample_rate;
}

void unloadAudio()
{
	alSourceStop(source);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	free(data);
}


// These are the rocket sync callback functions
void rocket_pause( int flag)
{
	if (flag)
    {
    	alSourcePause(source);
    }
	else
    {
    	alSourcePlay(source);
    }
}

void rocket_set_row( int row)
{
    double frow = row;// - 0.01;
	alSourcef(source, AL_SEC_OFFSET, frow / row_rate);
}

int rocket_is_playing()
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
