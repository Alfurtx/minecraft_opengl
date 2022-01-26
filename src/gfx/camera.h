#ifndef CAMERA_H_
#define CAMERA_H_

#include "../utils/types.h"
#include "window.h"
#include <cglm/cglm.h>

enum CameraMovement
{
        FORWARD,
        BACKWAR,
        LEFT,
        RIGHT,
};

/* Render Distance of CHUNK_SIZE_Z (16) * NUMBER_OF_CHUNKS_RENDERED (16)*/
const float CAMERA_RENDER_DISTANCE = 512.0f;
const float CAMERA_YAW         = -90.0f;
const float CAMERA_PITCH       = 0.0f;
const float CAMERA_SPEED       = 2.5f;
const float CAMERA_SENSITIVITY = 0.1f;
const float CAMERA_FOV         = 45.0f;

struct Camera
{
        vec3 position;
        vec3 front;
        vec3 up;
        vec3 right;
        vec3 world_up;

        float yaw;
        float pitch;

        float speed;
        float sensitivity;
};

extern struct Camera camera_create();
extern void          camera_get_view(struct Camera* camera, mat4 dest);
extern void          camera_get_projection(struct Camera* camera, mat4 dest);
extern void          camera_process_keyboard(struct Camera* camera, enum CameraMovement direction, float deltatime);
extern void          camera_process_mouse(struct Camera* camera, float x_off, float y_off);

#endif // CAMERA_H_
