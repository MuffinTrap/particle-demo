#ifdef PLATFORM_WII 
    #include <GL/opengx.h>
    #include <GL/glu.h>
#else
    #ifdef __APPLE__
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
        #include <GLUT/glut.h>
    #else
        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glut.h>
    #endif
#endif
#include <math.h>
#include "myrandom.h"
#include <stdlib.h>
#include <stdio.h>
#include "vector3.h"
extern GLuint textures[4];
float perlin(float x, float y);
float uniform_OrbitX;
float uniform_OrbitY;
float uniform_OrbitZ;
float uniform_Repulsion;
float uniform_RepulsionPower;
float uniform_Gravity;
float uniform_GravityPower;
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
int uniform_VisualSelector;
float uniform_FakeBokeh;
float uniform_ParticleR;
float uniform_ParticleG;
float uniform_ParticleB;
float uniform_ParticleA;

#define WIND_ENABLED TRUE
#define ORBIT_ENABLED TRUE

typedef struct {
    float x, y;
} float2;
float fract(float x) {
    return x - floor(x);
}
float hash(float p) {
    return fract(fract(p * 5.3983f) * fract(p * 0.4427f) * 95.4337f) * 2.0f - 1.0f;
}
// Utility functions
float max(float a, float b) {
    return a > b ? a : b;
}

float min(float a, float b) {
    return a < b ? a : b;
}

#define GL_SILENCE_DEPRECATION 1
#define PI 3.14159265358979323846


// Permutation table
static int p[512];

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
// Function to rotate a point around the Z-axis
void rotate_point_z(Vector3* p, double theta) {
    Vector3 rotated;
    rotated.x = p->x * cos(theta) - p->y * sin(theta);
    rotated.y = p->x * sin(theta) + p->y * cos(theta);
    rotated.z = p->z; // Z-coordinate remains unchanged
    p->x = rotated.x;
    p->y = rotated.y;
    p->z = rotated.z;
}

// Function to rotate a point around the Y-axis
void rotate_point_y(Vector3* p, double theta) {
    Vector3 rotated;
    rotated.x = p->x * cos(theta) + p->z * sin(theta);
    rotated.y = p->y; // Y-coordinate remains unchanged
    rotated.z = -p->x * sin(theta) + p->z * cos(theta);
    p->x = rotated.x;
    p->y = rotated.y;
    p->z = rotated.z;
}

// Function to rotate a point around the X-axis
void rotate_point_x(Vector3* p, double theta) {
    Vector3 rotated;
    rotated.x = p->x; // X-coordinate remains unchanged
    rotated.y = p->y * cos(theta) - p->z * sin(theta);
    rotated.z = p->y * sin(theta) + p->z * cos(theta);
    p->x = rotated.x;
    p->y = rotated.y;
    p->z = rotated.z;
}
float length3(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float length2(float2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vector3 Vector3_add(Vector3 a, Vector3 b) {
    Vector3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

Vector3 Vector3_sub(Vector3 a, Vector3 b) {
    Vector3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

Vector3 Vector3_scale(Vector3 v, float s) {
    Vector3 result = {v.x * s, v.y * s, v.z * s};
    return result;
}

Vector3 normalize(Vector3 v) {
    float len = length3(v);
    if (len == 0) return (Vector3){0.0, 0.0, 0.0};
    return Vector3_scale(v, 1.0f / len);
}

// Re-implementation of the provided functions
float fOpUnionRound(float a, float b, float r) {
    float2 u = {max(r - a, 0.0f), max(r - b, 0.0f)};
    return max(r, fmin(a, b)) - length2(u);
}

float fSphere(Vector3 p, float r) {
    return length3(p) - r;
}

float fTorus(Vector3 p, float smallRadius, float largeRadius) {
    float2 p_xz = {sqrtf(p.x * p.x + p.z * p.z) - largeRadius, p.y};
    return length2(p_xz) - smallRadius;
}

Vector3 posA, posB;
float getDistance(Vector3 position) {
    float dist = 1e10;
    //dist = min(fSphere(Vector3_sub(position, posA),uniform_Repulsion),dist);
   // dist = min(fSphere(Vector3_sub(position, posB),uniform_Repulsion),dist);
    dist = fTorus(position, uniform_EffectA, uniform_EffectB);
    dist = min(fSphere(position, uniform_EffectC),dist);
    return dist;
} 

#define epsilon 0.1f

Vector3 getSurfaceNormal(Vector3 position) {
    Vector3 e = {epsilon, 0.0f, 0.0f};
    Vector3 normalVector = {
        getDistance(Vector3_add(position, e)) - getDistance(Vector3_sub(position, e)),
        getDistance(Vector3_add(position, (Vector3){0.0f, epsilon, 0.0f})) - getDistance(Vector3_sub(position, (Vector3){0.0f, epsilon, 0.0f})),
        getDistance(Vector3_add(position, (Vector3){0.0f, 0.0f, epsilon})) - getDistance(Vector3_sub(position, (Vector3){0.0f, 0.0f, epsilon}))
    };
    return normalize(normalVector);
}

Vector3 camera_pos;
Vector3 viewDirection;
Vector3 right;
Vector3 up;

#define NUM_PARTICLES 512

float particleCloud_vertices[NUM_PARTICLES * 3 * 4];
float particleCloud_uv[NUM_PARTICLES * 2 * 4];
float particleCloud_colour[NUM_PARTICLES * 4 * 4];
uint16_t particleCloud_indices[NUM_PARTICLES * 4];

Vector3 particles[NUM_PARTICLES];
Vector3 velocity[NUM_PARTICLES];
float size[NUM_PARTICLES];
unsigned short lifetime[NUM_PARTICLES];
/*
r is the radius as a function of the angle 
a,b are the scaling factors
m controls the number of symmetries
n1,n2,n3 shape parameters*/
// Function to compute the Superformula
float superformula(float theta, float a, float b, float m, float n1, float n2, float n3) {
    float t1 = pow(fabs(cos(m * theta / 4.0) / a), n2);
    float t2 = pow(fabs(sin(m * theta / 4.0) / b), n3);
    return pow(t1 + t2, -1.0 / n1);
}
int tiktok = 0;
void generateSphere(float radius, int longitudeSegments, int latitudeSegments) {
    float lonStep = 2.0f * M_PI / longitudeSegments;
    float latStep = M_PI / latitudeSegments;
    int index = 0;
    tiktok++;
    if((tiktok&1) == 0)
    for (int i = 1; i <= latitudeSegments-1; i++) {
        float lat = M_PI / 2.0f - i * latStep;
        float xy = radius * cosf(lat);
        float z = radius * sinf(lat);

        for (int j = 1; j <= longitudeSegments-1; j++) {
            float lon = j * lonStep;
            float x = xy * cosf(lon);
            float y = xy * sinf(lon);

            /*normals.x = x / radius;
            normals.y = y / radius;
            normals.z = z / radius;*/

            particles[index].x = x;
            particles[index].y = z;
            particles[index].z = y;
            size[index++] = sinf(uniform_EffectA+y) / 2.0f + 0.5f;
        }
    }
}

void calculateCameraPlane(Vector3 cameraPosition, Vector3 cameraLookAt, Vector3 cameraUp) {
    // Calculate view direction vector
    viewDirection.x = cameraLookAt.x - cameraPosition.x;
    viewDirection.y = cameraLookAt.y - cameraPosition.y;
    viewDirection.z = cameraLookAt.z - cameraPosition.z;
    Vector3_Normalize(&viewDirection);

    // Calculate right vector (cross product of view direction and up vector)
    right.x = cameraUp.y * viewDirection.z - cameraUp.z * viewDirection.y;
    right.y = cameraUp.z * viewDirection.x - cameraUp.x * viewDirection.z;
    right.z = cameraUp.x * viewDirection.y - cameraUp.y * viewDirection.x;
    Vector3_Normalize(&right);

    // Calculate up vector (cross product of right vector and view direction)
    up.x = viewDirection.y * right.z - viewDirection.z * right.y;
    up.y = viewDirection.z * right.x - viewDirection.x * right.z;
    up.z = viewDirection.x * right.y - viewDirection.y * right.x;
    Vector3_Normalize(&up);

}

Vector3 getCenterPoint( float id, Vector3 seed) {
  float a0 = id + seed.x;
  float a1 = id + seed.y;
  float a2 = id + seed.z;
  Vector3 pos = {
    (sin(a0 * 0.39f) + sin(a0 * 0.73f) + sin(a0 * 1.27f)) / 3.0f,
    (sin(a1 * 0.43f) + sin(a1 * 0.37f) + cos(a1 * 1.73f)) / 3.0f,
    (sin(a2 * 0.39f) + sin(a2 * 0.37f) + cos(a2 * 1.27f)) / 3.0f};
  return pos;
}
Vector3 startColor = {
    183.0f/255.0f,
    0.0f/255.0f,
    179.0f/255.0f
};
Vector3 midColor = {
    178.0f/255.0f,
    180.0f/255.0f,
    0.0f/255.0f
};
Vector3 endColor = {
    0.0f/255.0f,
    195.0f/255.0f,
    198.0f/255.0f
};
// Function to interpolate between three values
float interpolate(float value1, float value2, float value3, float t) {
    // Check the value of t and interpolate accordingly
    if (t < 0.5) {
        // Interpolate between value1 and value2
        return value1 + (value2 - value1) * (t * 2);
    } else {
        // Interpolate between value2 and value3
        return value2 + (value3 - value2) * ((t - 0.5) * 2);
    }
}
void initParticles() {
    Vector3 topleft = {+right.x + up.x, +right.y + up.y, +right.z + up.z};
    Vector3 topright = {-right.x + up.x, -right.y + up.y, -right.z + up.z};
    Vector3 bottomleft = {+right.x - up.x, +right.y - up.y, +right.z - up.z};
    Vector3 bottomright = {-right.x - up.x, -right.y - up.y, -right.z - up.z};

    for (int i = 0; i < NUM_PARTICLES; ++i) {
        particles[i].x = (super_randf() * 2.0f - 1.0f) * 50.0f;
        particles[i].y = (super_randf() * 2.0f - 1.0f) * 50.0f;
        particles[i].z = (super_randf() * 2.0f - 1.0f) * 50.0f;
        velocity[i].x = (super_randf() * 2.0f - 1.0f);
        velocity[i].y = (super_randf() * 2.0f - 1.0f);
        velocity[i].z = (super_randf() * 2.0f - 1.0f);
        int a = i * 16;
        float r = interpolate( startColor.x, midColor.x, endColor.x, i/(float)NUM_PARTICLES);
        float g = interpolate( startColor.y, midColor.y, endColor.y, i/(float)NUM_PARTICLES);
        float b = interpolate( startColor.z, midColor.z, endColor.z, i/(float)NUM_PARTICLES);
        particleCloud_colour[a++] = r;
        particleCloud_colour[a++] = g;
        particleCloud_colour[a++] = b;
        particleCloud_colour[a++] = 1.0f;
        particleCloud_colour[a++] = r;
        particleCloud_colour[a++] = g;
        particleCloud_colour[a++] = b;
        particleCloud_colour[a++] = 1.0f;
        particleCloud_colour[a++] = r;
        particleCloud_colour[a++] = g;
        particleCloud_colour[a++] = b;
        particleCloud_colour[a++] = 1.0f;
        particleCloud_colour[a++] = r;
        particleCloud_colour[a++] = g;
        particleCloud_colour[a++] = b;
        particleCloud_colour[a++] = 1.0f;

        int vertexIndex = i * 12;
        particleCloud_vertices[vertexIndex++] = topleft.x + particles[i].x;
        particleCloud_vertices[vertexIndex++] = topleft.y + particles[i].y;
        particleCloud_vertices[vertexIndex++] = topleft.z + particles[i].z;

        particleCloud_vertices[vertexIndex++] = topright.x + particles[i].x;
        particleCloud_vertices[vertexIndex++] = topright.y + particles[i].y;
        particleCloud_vertices[vertexIndex++] = topright.z + particles[i].z;

        particleCloud_vertices[vertexIndex++] = bottomright.x + particles[i].x;
        particleCloud_vertices[vertexIndex++] = bottomright.y + particles[i].y;
        particleCloud_vertices[vertexIndex++] = bottomright.z + particles[i].z;

        particleCloud_vertices[vertexIndex++] = bottomleft.x + particles[i].x;
        particleCloud_vertices[vertexIndex++] = bottomleft.y + particles[i].y;
        particleCloud_vertices[vertexIndex++] = bottomleft.z + particles[i].z;

        vertexIndex = i * 8;

        particleCloud_uv[vertexIndex++] = 0.0f;
        particleCloud_uv[vertexIndex++] = 0.0f;

        particleCloud_uv[vertexIndex++] = 1.0f;
        particleCloud_uv[vertexIndex++] = 0.0f;

        particleCloud_uv[vertexIndex++] = 1.0f;
        particleCloud_uv[vertexIndex++] = 1.0f;

        particleCloud_uv[vertexIndex++] = 0.0f;
        particleCloud_uv[vertexIndex++] = 1.0f;

        int indexBase = i * 4;
        particleCloud_indices[indexBase + 0] = indexBase + 0;
        particleCloud_indices[indexBase + 1] = indexBase + 1;
        particleCloud_indices[indexBase + 2] = indexBase + 2;
        particleCloud_indices[indexBase + 3] = indexBase + 3;
                size[i] = 1.8f + sinf(i);
    }
}
// Placeholder for the Perlin noise function (implement or include your Perlin noise function here)
float perlinNoise(float x, float y, float z) {
    return perlin(x,y) + perlin(y, z); 
}

// Function to compute the curl noise based on Perlin noise
Vector3 curlNoise(Vector3 p) {
    const float e = 0.1f; // Small epsilon value for finite differences

    // Compute partial derivatives using central differences
    float dx = perlinNoise(p.x + e, p.y, p.z) - perlinNoise(p.x - e, p.y, p.z);
    float dy = perlinNoise(p.x, p.y + e, p.z) - perlinNoise(p.x, p.y - e, p.z);
    float dz = perlinNoise(p.x, p.y, p.z + e) - perlinNoise(p.x, p.y, p.z - e);

    // Calculate curl vector
    Vector3 curl = {dy - dz, dz - dx, dx - dy};
    return curl;
}
void resetParticle(int i) {
    float x = (right.x + up.x) * hash(i + 0.1f);
    float y = (right.y + up.y) * hash(i + 12.333f);
    float z = (right.z + up.z) * hash(i + 1.4f);
    Vector3 direction = {
        x - viewDirection.x, 
        y - viewDirection.y, 
        z - viewDirection.z};
    Vector3_Normalize(&direction);
    particles[i].x = camera_pos.x - direction.x * 5.0f;
    particles[i].y = camera_pos.y - direction.y * 5.0f;
    particles[i].z = camera_pos.z - direction.z * 5.0f;
    velocity[i].x = direction.x;
    velocity[i].y = direction.y;
    velocity[i].z = direction.z;
    lifetime[i] = 16 + (rand()&15);
    size[i] = 0.0f; 
}
float effect_time;
int flip = 0;
int flip2 = 0;
int setonce = 0;
int prevcount = 0;
void updateQuads( GLdouble cameraX, GLdouble cameraY, GLdouble cameraZ) {
    flip ^= 1;
    camera_pos.x = cameraX;
    camera_pos.y = cameraY;
    camera_pos.z = cameraZ;
    Vector3 effect = {
        uniform_EffectA,
        uniform_EffectB,
        uniform_EffectC
    };
    float camera_length = Vector3_Length(&camera_pos)*8.0f;
    if(uniform_VisualSelector == 0.0f) {
        Vector3 topleft;
        Vector3 topright;
        Vector3 bottomleft;
        Vector3 bottomright;
        for (int i = 0; i < min(NUM_PARTICLES,(int)uniform_ParticleA+1); ++i) {
            if((i&3)==0) {
                Vector3 right = { 1.0f, 0.0f, 0.0f};
                Vector3 up = { 0.0f, 1.0f, 0.0f};
                right = Vector3_Scale(&right, size[i]);
                up = Vector3_Scale(&up, size[i]);
                rotate_point_x(&right, i * 2.1f);
                rotate_point_y(&right, i * 1.01112f);
                //rotate_point_z(&right, i * 0.21112f + time * 3.141592f);
                rotate_point_x(&up, i * 2.1f);
                rotate_point_y(&up, i * 1.01112f);
                //rotate_point_z(&up, i * 0.21112f + time * 3.141592f);
                topleft = Vector3_Create(+right.x + up.x, +right.y + up.y, +right.z + up.z);
                topright = Vector3_Create(-right.x + up.x, -right.y + up.y, -right.z + up.z);
                bottomleft = Vector3_Create(+right.x - up.x, +right.y - up.y, +right.z - up.z);
                bottomright = Vector3_Create(-right.x - up.x, -right.y - up.y, -right.z - up.z);
            }

            int vertexIndex = i * 12; // 3 coordinates per vertex, 4 vertices per particle

            particleCloud_vertices[vertexIndex++] = topleft.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = topleft.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = topleft.z + particles[i].z;

            particleCloud_vertices[vertexIndex++] = topright.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = topright.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = topright.z + particles[i].z;

            particleCloud_vertices[vertexIndex++] = bottomright.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = bottomright.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = bottomright.z + particles[i].z;

            particleCloud_vertices[vertexIndex++] = bottomleft.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = bottomleft.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = bottomleft.z + particles[i].z;
        }
    } else {
        Vector3 cloneRight = {
            right.x,right.y,right.z
        };
        Vector3 cloneUp = {
            up.x,up.y,up.z
        };
            Vector3 topleft = {+right.x + up.x, +right.y + up.y, +right.z + up.z};
            Vector3 topright = {-right.x + up.x, -right.y + up.y, -right.z + up.z};
            Vector3 bottomleft = {+right.x - up.x, +right.y - up.y, +right.z - up.z};
            Vector3 bottomright = {-right.x - up.x, -right.y - up.y, -right.z - up.z};
        for (int i = 0; i < min(NUM_PARTICLES,(int)uniform_ParticleA); ++i) {
            if((i&1)==0) {
                right = Vector3_Scale(&cloneRight, size[i]);
                up = Vector3_Scale(&cloneUp, size[i]);
                
                topleft = Vector3_Create(+right.x + up.x, +right.y + up.y, +right.z + up.z);
                topright = Vector3_Create(-right.x + up.x, -right.y + up.y, -right.z + up.z);
                bottomleft = Vector3_Create(+right.x - up.x, +right.y - up.y, +right.z - up.z);
                bottomright = Vector3_Create(-right.x - up.x, -right.y - up.y, -right.z - up.z);
            }
            int vertexIndex = i * 12; // 3 coordinates per vertex, 4 vertices per particle

            particleCloud_vertices[vertexIndex++] = topleft.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = topleft.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = topleft.z + particles[i].z;

            particleCloud_vertices[vertexIndex++] = topright.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = topright.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = topright.z + particles[i].z;

            particleCloud_vertices[vertexIndex++] = bottomright.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = bottomright.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = bottomright.z + particles[i].z;

            particleCloud_vertices[vertexIndex++] = bottomleft.x + particles[i].x;
            particleCloud_vertices[vertexIndex++] = bottomleft.y + particles[i].y;
            particleCloud_vertices[vertexIndex++] = bottomleft.z + particles[i].z;
        }
    }
    int a = 0;
    switch(uniform_VisualSelector) {
        case 0:
            for (int i = 0; i < NUM_PARTICLES; ++i) {
                Vector3 localVelocity = velocity[i];
                particles[i] = Vector3_Add(&particles[i], &localVelocity);
                //if(Vector3_Length(&particles[i])>camera_length || lifetime[i]-- == 0) {
                    //resetParticle( i);
                //}
            }
            break;
        case 1:
            for (int i = prevcount; i < NUM_PARTICLES; ++i) {
                particles[i] = getCenterPoint( i * uniform_EffectD, effect);
                particles[i] = Vector3_Scale(&particles[i], 10.0f);
                size[i] = sin(particles[i].x+particles[i].y+particles[i].z)/3.0f + 0.5f;
                if(setonce==0) {
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 0.9f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 0.9f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 0.9f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 1.0f;
                    particleCloud_colour[a++] = 0.9f;
                }
                setonce = 1;
                if(prevcount>uniform_ParticleA) {
                    prevcount=uniform_ParticleA;
                } else {
                    prevcount=0;
                }
            }
            break;
        case 2: 
            /*
            r is the radius as a function of the angle 
            a,b are the scaling factors
            m controls the number of symmetries
            n1,n2,n3 shape parameters*/
            // Function to compute the Superformula
            // superformula(theta, a, b, m, n1, n2, n3);
            /*for (int i = 0; i < NUM_PARTICLES; ++i) {
                float theta = 2.0 * M_PI * i / NUM_PARTICLES;
                float r = superformula(theta, 
                                    uniform_EffectA, 
                                    uniform_EffectB, 
                                    uniform_EffectC, 
                                    uniform_EffectD, 
                                    uniform_EffectE, 
                                    uniform_EffectF);
            }*/

            generateSphere(9.0f, 17, 24);
            break;
        case 3:
            for (int i = prevcount; i < NUM_PARTICLES; ++i) {
                Vector3 localVelocity = velocity[i];
                particles[i] = Vector3_Add(&particles[i], &localVelocity);
                size[i] = 1.8f + uniform_FakeBokeh * Vector3_Length(&particles[i]);
                //if(Vector3_Length(&particles[i])>camera_length || lifetime[i]-- == 0) {
                    //resetParticle( i);
                //}
            }
                if(prevcount>uniform_ParticleA) {
                    prevcount=uniform_ParticleA;
                } else {
                    prevcount=0;
                }
            break;
        case 5:
            for (int i = 0; i < NUM_PARTICLES; ++i) {
                //Vector3 localVelocity = Vector3_Scale(&velocity[i], dt);
                //particles[i] = Vector3_Add(&particles[i], &localVelocity);
                if(Vector3_Length(&particles[i])>camera_length) {
                    resetParticle( i);
                }
            }
            break;
    }
}

float signnum_c(float x) {
  if (x > 0.0f) return 1.0f;
  if (x < 0.0f) return -1.0f;
  return x;
}
float old_uniform_EffectA;
float old_uniform_EffectB;
float old_uniform_EffectC;
float old_uniform_EffectD;
float old_uniform_EffectE;
float old_uniform_EffectF;
const Vector3 world_up = {0.0f, 1.0f, 0.0f};
const Vector3 world_right = {1.0f, 0.0f, 0.0f};
const Vector3 world_forward = {0.0f, 0.0f, 1.0f};
void trace(float dt) {
    int start2 = flip2 * (NUM_PARTICLES / 4.0f);
    int end2 = start2 + (NUM_PARTICLES / 4.0f);
    uniform_OrbitX = uniform_OrbitX * dt * 0.1f;
    uniform_OrbitY = uniform_OrbitY * dt * 0.1f;
    uniform_OrbitZ = uniform_OrbitZ * dt * 0.1f;
    int different = 0;
    #ifdef WIND_ENABLED
        Vector3 wind = {uniform_WindX * dt, uniform_WindY * dt, uniform_WindZ * dt};
    #endif
    for (int i = start2; i < end2; ++i) {
        if(different==0) {
            float dist = getDistance(particles[i]);
            Vector3 direction = getSurfaceNormal(particles[i]);
            direction = Vector3_Scale(&direction, -dist);
            #ifdef WIND_ENABLED
                Vector3 tangent = Vector3_Cross(&direction, &world_up);
                Vector3 tangent2 = Vector3_Cross(&direction, &world_right);
                Vector3 tangent3 = Vector3_Cross(&direction, &world_forward);
            #endif
            #ifdef WIND_ENABLED
                Vector3 curl = particles[i];
                curl = Vector3_Add(&curl, &particles[i]);
                curl = curlNoise(curl);
                curl = Vector3_Multiply(&curl, &wind);
                curl = Vector3_Add(&curl, &tangent);
                curl = Vector3_Add(&curl, &tangent2);
                curl = Vector3_Add(&curl, &tangent3);
                curl = Vector3_Scale(&curl, dt);
                direction = Vector3_Add(&direction, &curl);
            #endif
            #ifdef ORBIT_ENABLED
                tangent = Vector3_Scale(&tangent, uniform_OrbitX);
                tangent2 = Vector3_Scale(&tangent2, uniform_OrbitY);
                tangent3 = Vector3_Scale(&tangent3, uniform_OrbitZ);
                direction = Vector3_Add(&direction, &tangent);
                direction = Vector3_Add(&direction, &tangent2);
                direction = Vector3_Add(&direction, &tangent3);
            #endif
            particles[i] = Vector3_Add(&particles[i], &direction);
        }
    }
    old_uniform_EffectA = uniform_EffectA;
    old_uniform_EffectB = uniform_EffectB;
    old_uniform_EffectC = uniform_EffectC;
    old_uniform_EffectD = uniform_EffectD;
    old_uniform_EffectE = uniform_EffectE;
    old_uniform_EffectF = uniform_EffectF;
}
void updateForces(float dt) {
    effect_time += dt;
    posA.x = uniform_EffectA;
    posA.y = uniform_EffectB;
    posA.z = uniform_EffectC;
    posB.x = uniform_EffectD;
    posB.y = uniform_EffectE;
    posB.z = uniform_EffectF;
    uniform_OrbitX = uniform_OrbitX;
    uniform_OrbitY = uniform_OrbitY;
    uniform_OrbitZ = uniform_OrbitZ;
    Vector3 orbit = {
        uniform_OrbitX * dt, uniform_OrbitY * dt, uniform_OrbitZ * dt
    };
    flip2 = (flip2 + 1) & 3;
    #ifdef WIND_ENABLED
        Vector3 wind = {uniform_WindX * dt, uniform_WindY * dt, uniform_WindZ * dt};
    #endif
    for (int i = 0; i < min(NUM_PARTICLES,(int)uniform_ParticleA); ++i) {
        if(((i+flip2)&3) == 0) continue;
        Vector3 curl = Vector3_Scale(&particles[i], 0.01f);
        curl = curlNoise(curl);
        curl = Vector3_Multiply(&curl, &orbit);
        curl = Vector3_Add(&curl, &wind);
        velocity[i] = curl;
        particles[i].x = fmod(particles[i].x + 50.0f,100.0f) - 50.0f ;
        particles[i].y = fmod(particles[i].y + 50.0f,100.0f) - 50.0f ;
        particles[i].z = fmod(particles[i].z + 50.0f,100.0f) - 50.0f;
    }
}
void drawParticles() {
#ifdef N64
    rdpq_debug_log_msg("Particles");
#endif
    glPushMatrix();
    // Enable alpha testing
    glDisable(GL_ALPHA_TEST);

    // Set the alpha test function (e.g., only pass fragments with alpha > 0.5)
    //glAlphaFunc(GL_GREATER, 0.333f);

    glDisable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);
    // Enable blending if you also want to use blending
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Enable texturing
    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    //glBindTexture(GL_TEXTURE_2D, textures[tex_id]); 
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, particleCloud_vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, particleCloud_uv);
    glColorPointer(4, GL_FLOAT, 0, particleCloud_colour);

    glDrawElements(GL_QUADS, min(NUM_PARTICLES,(int)uniform_ParticleA) * 4, GL_UNSIGNED_SHORT, particleCloud_indices);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_COLOR_MATERIAL);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); 
    glDisable(GL_ALPHA_TEST);
    /*
    glPointSize(1.0f);
    if(uniform_VisualSelector == 5) {
        glEnable(GL_BLEND);
        // Draw lines
        glBegin(GL_LINES);                                  // Begin drawing lines
        right = Vector3_Scale(&right,0.02f);
        up = Vector3_Scale(&up,0.02f);
        for (int i = 0; i <  min(NUM_PARTICLES,(int)uniform_ParticleA); i++) {
            glColor4f(1.0f,1.0f,1.0f,1.0f);
            glVertex3f(particles[i].x,particles[i].y,particles[i].z);
            glColor4f(0.0f,0.0f,0.0f,0.0f);
            glVertex3f(particles[i].x + velocity[i].x * 0.25f,particles[i].y + velocity[i].y * 0.25f,particles[i].z + velocity[i].z * 0.25f);

        }
        glEnd();
        glDisable(GL_BLEND); 
    }*/
    glPopMatrix();
}
