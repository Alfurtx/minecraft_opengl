#ifndef CAMERA_H_
#define CAMERA_H_

#include "../utils/types.h"
#include "window.h"
#include <cglm/cglm.h>

enum CameraMovement
{
        CAM_MOVE_FORWARD,
        CAM_MOVE_BACKWARD,
        CAM_MOVE_LEFT,
        CAM_MOVE_RIGHT,
        CAM_MOVE_UP,
        CAM_MOVE_DOWN,
};

/* Render Distance of CHUNK_SIZE_Z (16) * NUMBER_OF_CHUNKS_RENDERED (16)*/
extern const float CAMERA_RENDER_DISTANCE;
extern const float CAMERA_YAW;
extern const float CAMERA_PITCH;
extern const float CAMERA_SPEED;
extern const float CAMERA_SENSITIVITY;
extern const float CAMERA_FOV;

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

        float lastx;
        float lasty;
        bool firstmouse;
};

extern void camera_init(struct Camera* camera);
extern void camera_get_view(struct Camera* camera, mat4 dest);
extern void camera_get_projection(struct Camera* camera, mat4 dest);
extern void camera_proccess_keyboard(struct Camera* camera, enum CameraMovement direction, float deltatime);
extern void camera_proccess_mouse(struct Camera* camera, float xoff, float yoff);

#endif // CAMERA_H_
