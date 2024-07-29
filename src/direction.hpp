#ifndef DIRECTION_H
#define DIRECTION_H

#define SYNC_FILE_H "src/sync_data.h"
#define SYNC_FILE_CPP "src/sync_data.cpp"

#include <stdbool.h>

#ifdef GEKKO
#include <mgdl-wii.h>
#endif

extern "C" {
#ifdef GEKKO
    extern gdl::Sound musicSound;
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
    void setupDirection(double beatsPerMin, int rowsPerBeat);

    // These are different for each platform
    int loadAudio(const char* filename);
    void playAudio();
    void updateAudio();
    void unloadAudio();

    // Functions called by demo

    double getTime();
    bool isMusicEnabled();
    void frameCounterIncrement();
    double get_row();

    // Functions linked to Rocket
    void rocket_pause(int flag);
    void rocket_set_row( int row);
    int rocket_is_playing();

} // extern "C"

#endif
