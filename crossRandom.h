#pragma once

#include <cstdlib>

extern "C" {
float GetRandomFloat(float min, float max)
{
    float range = max - min;
	return min + range * ((float)rand()/(float)RAND_MAX);
}
int GetRandomInt(int min, int max)
{
    int range = max - min;
	if (range == 0)
	{
		return min;
	}
	return min + rand()%range;
}

}
