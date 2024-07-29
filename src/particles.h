#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../crossOpenGL.h"
#include "myrandom.h"
#include "vector3.h"


extern GLuint textures[4];
#define GL_SILENCE_DEPRECATION 1
#define PI 3.14159265358979323846

// Function to return the sign of a number
int sign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}
// Permutation table
static int p[512];

extern "C" {

void init_perlin(int seed) {
    int permutation[] = {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,
    36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,
    203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,
    40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,
    130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,
    118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,
    152,2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253,19,98,108,110,79,113,
    224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
    81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,
    50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,
    156,180};
    
    for (int i = 0; i < 256; i++) {
        p[256 + i] = p[i] = permutation[i];
    }
}

// Fade function as defined by Ken Perlin
static float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
static float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

// Gradient function calculates the dot product between a pseudorandom gradient vector and the vector from the input coordinate to the 8 surrounding points in its unit cube
static float grad(int hash, float x, float y) {
    int h = hash & 7;      // Convert low 3 bits of hash code
    float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    float v = h < 4 ? y : x;  // and compute the dot product with (x,y).
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}

// 2D Perlin noise
float perlin(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floor(x);
    y -= floor(y);
    float u = fade(x);
    float v = fade(y);
    int A = p[X] + Y, AA = p[A], AB = p[A + 1], B = p[X + 1] + Y, BA = p[B], BB = p[B + 1];

    return lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)),
                   lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));
}

float uniform_OrbitX;
float uniform_OrbitY;
float uniform_OrbitZ;
float uniform_Gravity;
float uniform_GravityPower;
float uniform_Repulsion;
float uniform_RepulsionPower;
float uniform_Friction;
float uniform_EffectA;
float uniform_EffectB;
float uniform_EffectC;
float uniform_EffectD;
float uniform_EffectE;
float uniform_EffectF;
float uniform_WindX;
float uniform_WindY;
float uniform_WindZ;
float uniform_ParticleCount;
int uniform_VisualSelector;
float uniform_FakeBokeh;
#define WIND_ENABLED TRUE
#define ORBIT_ENABLED TRUE



#define NUM_PARTICLES 1000
#define SPHERE_RADIUS 1.0f
#define STICK_DISTANCE 0.01f
#define FRICTION 0.98f  // Friction coefficient
#define MAX_LIFETIME 100.0f // Maximum lifetime of a particle

typedef struct {
    Vector3 position;
    Vector3 velocity;
    float lifetime; // Lifetime of the particle
    float age; // Current age of the particle
    bool active; // If the particle is active
} Particle;

Particle particles[NUM_PARTICLES];

// Function to initialize a single particle
void initParticle(Particle *p) {
    p->position.x = (rand() / (float)RAND_MAX) * 4.0f - 2.0f;
    p->position.y = (rand() / (float)RAND_MAX) * 4.0f - 2.0f;
    p->position.z = (rand() / (float)RAND_MAX) * 4.0f - 2.0f;
    p->velocity.x = p->velocity.y = p->velocity.z = 0.0f;
    p->lifetime = (rand() / (float)RAND_MAX) * MAX_LIFETIME;
    p->age = 0.0f;
    p->active = true;
}

// Function to initialize all particles
void initParticles() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        initParticle(&particles[i]);
    }
}

// Function to compute the SDF for a sphere
float sphereSDF(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z) - SPHERE_RADIUS;
}
float getDistance(Vector3 position) {
    return sphereSDF(position.x + uniform_EffectA, position.y + uniform_EffectB, position.z + uniform_EffectC);
}
// Define epsilon for numerical stability
#define EPSILON 1e-6f

Vector3 getSurfaceNormal(Vector3 position) {
    Vector3 e = {EPSILON, 0.0f, 0.0f};
    Vector3 e2 = {0.0f, EPSILON, 0.0f};
    Vector3 e3 = {0.0f, 0.0f, EPSILON};
    Vector3 normalVector = {
        getDistance(Vector3_Add(&position, &e)) - getDistance(Vector3_Sub(&position, &e)),
        getDistance(Vector3_Add(&position, &e2)) - getDistance(Vector3_Sub(&position, &e2)),
        getDistance(Vector3_Add(&position, &e3)) - getDistance(Vector3_Sub(&position, &e3))
    };

    // Normalize the normalVector to ensure it has unit length
    float length = sqrtf(normalVector.x * normalVector.x +
                          normalVector.y * normalVector.y +
                          normalVector.z * normalVector.z);
    if (length > 0.0f) {
        normalVector.x /= length;
        normalVector.y /= length;
        normalVector.z /= length;
    }

    return normalVector;
}

const Vector3 world_up = {0.0f, 1.0f, 0.0f};
const Vector3 world_right = {1.0f, 0.0f, 0.0f};
const Vector3 world_forward = {0.0f, 0.0f, 1.0f};
void updateParticles(float dt) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle *p = &particles[i];

        if (!p->active) {
            initParticle(p);
            continue;
        }

        Vector3 direction = getSurfaceNormal(p->position);
        #ifdef ORBIT_ENABLED
            Vector3 tangent = Vector3_Cross(&direction, &world_up);
            Vector3 tangent2 = Vector3_Cross(&direction, &world_right);
            Vector3 tangent3 = Vector3_Cross(&direction, &world_forward);
                
            tangent = Vector3_Scale(&tangent, uniform_OrbitX);
            tangent2 = Vector3_Scale(&tangent2, uniform_OrbitY);
            tangent3 = Vector3_Scale(&tangent3, uniform_OrbitZ);
            direction = Vector3_Add(&direction, &tangent);
            direction = Vector3_Add(&direction, &tangent2);
            direction = Vector3_Add(&direction, &tangent3);
        #endif
        // Compute the distance from the particle to the SDF surface
        float dist = getDistance(p->position);
        if (dist < 0.0f) {  // If particle is inside the SDF
            // Compute the surface normal
            Vector3 normal = getSurfaceNormal(p->position);

            // Move the particle to the surface of the SDF
            float correctionDistance = -dist;
            Vector3 corrected = Vector3_Scale(&normal, correctionDistance);
            p->position = Vector3_Add(&p->position, &corrected);
        } else {
            float gravity = -uniform_Gravity / (dist * uniform_GravityPower + 1.0f);
            float repulsion = uniform_Repulsion / (dist * uniform_RepulsionPower + 1.0f);

            direction = Vector3_Scale(&direction, (gravity+repulsion) * dt);
        }
        particles[i].velocity = Vector3_Add(&particles[i].velocity, &direction);
        particles[i].velocity = Vector3_Scale(&particles[i].velocity, 1.0f - uniform_Friction * dt);
        Vector3 velocityScaled = Vector3_Scale(&particles[i].velocity, dt);
        particles[i].position = Vector3_Add(&particles[i].position,&velocityScaled);
    }
}


// Function to display particles
void displayParticles() {
    glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_PARTICLES; i++) {
        if (particles[i].active) {
            glVertex3f(particles[i].position.x, particles[i].position.y, particles[i].position.z);
        }
    }
    glEnd();
    if(uniform_EffectF==0) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
    for (int i = 0; i < NUM_PARTICLES; i+=uniform_EffectF) {
        glColor4f(1.0f,0.4f,0.0f, sin(i+uniform_EffectE)*uniform_EffectD);
        if (particles[i].active) {
            glVertex3f(particles[i].position.x, particles[i].position.y, particles[i].position.z);
        }
    }
    glEnd();
}

} // End extern "C"
