#ifndef DIRECTION_H
#define DIRECTION_H

#include <stdbool.h>

extern "C" {

#ifdef GEKKO
    // TODO
#else
    #include <sndfile.h>
    #include <AL/al.h>
    #include <AL/alc.h>
    extern SNDFILE* sndfile;
    extern ALCdevice* device;
    extern ALCcontext* context;
    extern ALuint buffer, source;
    extern short* data;
    extern struct sync_cb cb;
#endif

    // Functions called by platform
    void setupDirection();
    int loadAudio(const char* filename);
    void playAudio();
    void updateAudio();

    // Functions called by demo

    double getTime();
    bool isMusicEnabled();
    void frameCounterIncrement();
    double get_row();

    // Functions linked to Rocket
    void pause(int flag);
    void set_row( int row);
    int is_playing();

} // extern "C"

#endif
