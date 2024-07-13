#define GL_SILENCE_DEPRECATION 1
#include <stdbool.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#ifdef GEKKO
    //TODO: 
#else
    #include <sndfile.h>
    #include <AL/al.h>
    #include <AL/alc.h>
#endif
static const bool music_enabled = true;
/***AUDIO PUBLIC***/

static double __attribute__((__unused__)) time_secs = 0.0;
static double __attribute__((__unused__)) time_secs_offset = 0.0f;
static int time_frames = 0;
double bpm; /* beats per minute */
int rpb; /* rows per beat */
double row_rate;
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

#ifdef GEKKO
    void audio_poll(void) { 

    //TODO: 
    }
#else
    // Create an OpenAL buffer and source
    ALuint buffer, source;
    ALCcontext* context;
    SNDFILE* sndfile;
    short* data;
    ALCdevice* device;
    SF_INFO sfinfo;
#endif

void updateAudio() {
    #ifdef GEKKO
    //TODO: 
        time_secs = 0.0;
    #else
        ALint sampleOffset;
        alGetSourcei(source, AL_SAMPLE_OFFSET, &sampleOffset);

        // Assuming you've already opened the audio file using sf_open
        sf_command(sndfile, SFC_GET_LOG_INFO, &sfinfo, sizeof(sfinfo));
        double sampleRate = 44100.0;
        time_secs = (float)sampleOffset / sampleRate;
    #endif
}

double get_row() 
{
	return time_secs * row_rate + 0.016;
}
void pause( int __attribute__((__unused__)) flag) 
{
    printf("pause %i\n", flag);
#ifndef GEKKO
	if (flag)
    	alSourcePause(source);
	else
    	alSourcePlay(source);
#endif
    set_row(get_row());
}
void set_row( int __attribute__((__unused__)) row)
{
    printf("set_row %i\n", row);
#ifndef GEKKO
    double frow = row - 0.01;
	alSourcef(source, AL_SEC_OFFSET, frow / row_rate);
#endif
}

int is_playing() 
{
#ifndef GEKKO
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
#endif
    return -1;
}