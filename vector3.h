#ifndef VECTOR3_H
#define VECTOR3_H
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

typedef struct Vector3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Vector3;
typedef struct Vector2 {
    GLfloat x;
    GLfloat y;
} Vector2;


Vector3 Vector3_Create(GLfloat x, GLfloat y, GLfloat z);
Vector3 Vector3_Add(const Vector3* v1, const Vector3* v2);
Vector3 Vector3_Sub(const Vector3* v1, const Vector3* v2);
GLfloat Vector3_Dot(const Vector3* v1, const Vector3* v2);
Vector3 Vector3_Cross(const Vector3* v1, const Vector3* v2);
GLfloat Vector3_Length(const Vector3* v);
void Vector3_Normalize(Vector3* v);
Vector3 Vector3_Multiply(const Vector3* v1, const Vector3* v2);
Vector3 Vector3_Scale(const Vector3* v1, float scale);
void Vector3_Reflect(const Vector3* I, const Vector3* N, Vector3* result);

#endif
