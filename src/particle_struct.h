#pragma once

#include "vector3.h"

struct Particle {
    Vector3 position;
    Vector3 velocity;
    float r,g,b,a;
    float lifetime; // Lifetime of the particle
    float age; // Current age of the particle
    bool active; // If the particle is active
};
