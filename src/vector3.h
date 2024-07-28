#ifndef VECTOR3_H
#define VECTOR3_H

#ifdef N64
    #include <libdragon.h>
    #include <GL/gl.h>
    #include <fmath.h>
#else
    #include <OpenGL/gl.h>
    #include <math.h>
#endif

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
