#ifndef UTILS_H_
#define UTILS_H_

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cglm/cglm.h>

// NOTE(fonsi): recordar que {0, 0, -1} es Norte
enum Direction
{
        NORTH = 0,
        SOUTH,
        EAST,
        WEST,
        UP,
        DOWN,
        DIRECTION_COUNT
};

const vec3 DIRECTION_VEC[6] = {
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
};

#define DIRECTIONVEC(direction) (DIRECTION_VEC[direction])

#endif // UTILS_H_
