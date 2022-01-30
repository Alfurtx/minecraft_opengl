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

extern void direction_init();

extern vec3 DIRECTION_VEC[6];

#endif // DIRECTION_H_
