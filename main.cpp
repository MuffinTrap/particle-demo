#include "platform.h"
 
int main(int argc, char** argv)
{
    bool useRocket = true;
    Platform demoPlatform;
    demoPlatform.Init(argc, argv, Screen4x3, useRocket);

    //

    const char* filename = "data/numbers.wav"; // Replace with your WAV file path
    demoPlatform.LoadMusic(filename, 92.0, 8);
    //

    demoPlatform.RunMainLoop();
}
