#include "direction.h"

vec3 DIRECTION_VEC[6];

void
direction_init()
{
        glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, DIRECTION_VEC[0]);
        glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, DIRECTION_VEC[1]);
        glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, DIRECTION_VEC[2]);
        glm_vec3_copy((vec3){-1.0f, 0.0f, 0.0f}, DIRECTION_VEC[3]);
        glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, DIRECTION_VEC[4]);
        glm_vec3_copy((vec3){0.0f, -1.0f, 0.0f}, DIRECTION_VEC[5]);
}
