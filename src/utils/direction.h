#ifndef DIRECTION_H_
#define DIRECTION_H_

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

extern const vec3 DIRECTION_VEC[6];

#define DIRECTIONVEC(direction) (DIRECTION_VEC[direction])

#endif // DIRECTION_H_
