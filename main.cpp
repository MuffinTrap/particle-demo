#include "platform.h"
 
int main(int argc, char** argv)
{
    bool useRocket = true;
    Platform demoPlatform;
    demoPlatform.Init(argc, argv, useRocket);
    const char* filename = "test_music.wav"; // Replace with your WAV file path
    demoPlatform.LoadMusic(filename);

    demoPlatform.RunMainLoop();
}
