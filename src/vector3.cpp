#include "vector3.h"

Vector3 Vector3_Create(GLfloat x, GLfloat y, GLfloat z) {
    Vector3 result = {x, y, z};
    return result;
}

Vector3 Vector3_Add(const Vector3* v1, const Vector3* v2) {
    Vector3 v3 = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};
    return v3;
}

Vector3 Vector3_Sub(const Vector3* v1, const Vector3* v2) {
    Vector3 v3 = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};
    return v3;
}

GLfloat Vector3_Dot(const Vector3* v1, const Vector3* v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

Vector3 Vector3_Cross(const Vector3* v1, const Vector3* v2) {
    Vector3 result;
    result.x = v1->y * v2->z - v1->z * v2->y;
    result.y = v1->z * v2->x - v1->x * v2->z;
    result.z = v1->x * v2->y - v1->y * v2->x;
    return result;
}

Vector3 Vector3_Multiply(const Vector3* v1, const Vector3* v2) {
    Vector3 result;
    result.x = v1->x * v2->x;
    result.y = v1->y * v2->y;
    result.z = v1->z * v2->z;
    return result;
}

Vector3 Vector3_Scale(const Vector3* v1, float scale) {
    Vector3 result;
    result.x = v1->x * scale;
    result.y = v1->y * scale;
    result.z = v1->z * scale;
    return result;
}

GLfloat Vector3_Length(const Vector3* v) {
    float madd = v->x * v->x + v->y * v->y + v->z * v->z;
    return sqrtf(madd);
}

void Vector3_Normalize(Vector3* v) {
    GLfloat length = Vector3_Length(v);
    if (length != 0.0f) {
        GLfloat invLength = 1.0f / length;
        v->x *= invLength;
        v->y *= invLength;
        v->z *= invLength;
    }
}

void Vector3_Reflect(const Vector3* I, const Vector3* N, Vector3* result) {
    GLfloat dotProduct = Vector3_Dot(N, I);

    result->x = I->x - 2.0f * dotProduct * N->x;
    result->y = I->y - 2.0f * dotProduct * N->y;
    result->z = I->z - 2.0f * dotProduct * N->z;
}
