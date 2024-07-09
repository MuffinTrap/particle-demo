#ifndef DIRECTION_H
#define DIRECTION_H
    #include <stdbool.h> 

#ifndef GEKKO
    #include <sndfile.h>
    #include <AL/al.h>
    #include <AL/alc.h>
    #include "rocket/sync.h"
    extern SNDFILE* sndfile;
    extern ALCdevice* device;
    extern ALCcontext* context;
    extern ALuint buffer, source;
    extern short* data;
    extern struct sync_cb cb;
#else
    void audio_poll(void);
    extern wav64_t music_wav;
#endif
    void setupDirection();
    void updateAudio();
    double getTime();
    bool isMusicEnabled();
    void frameCounterIncrement();
    double get_row();

    double get_row();
    void pause(int flag);
    void set_row( int row);

    int is_playing();
#endif