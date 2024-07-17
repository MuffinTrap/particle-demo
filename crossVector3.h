#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

// This file is in case glm is too slow on the wii.
// Enabling the typedef will change glm::vec3s to guVector
// which has faster routines on Wii.

#if 0

#ifdef GEKKO
typedef glm::vec3 guVector
#endif


#endif
