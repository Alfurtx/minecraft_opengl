#include "../utils/types.h"
#include "camera.h"
#include "gfx.h"

const float CAMERA_RENDER_DISTANCE = 100.0f;
const float CAMERA_YAW             = -90.0f;
const float CAMERA_PITCH           = 0.0f;
const float CAMERA_SPEED           = 2.5f;
const float CAMERA_SENSITIVITY     = 0.1f;
const float CAMERA_FOV             = 45.0f;

internal void camera_update_vectors(struct Camera* camera);

void
camera_init(struct Camera* camera)
{
        glm_vec3_copy((vec3){-1.0f, 9.0f, 1.0f}, camera->position);
        glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->world_up);
        glm_vec3_copy((vec3){-0.7f, -0.1f, 0.67f}, camera->front);
        camera->yaw         = CAMERA_YAW;
        camera->firstmouse  = false;
        camera->pitch       = CAMERA_PITCH;
        camera->speed       = CAMERA_SPEED;
        camera->sensitivity = CAMERA_SENSITIVITY;
        camera->lastx       = (float) WINDOW_SCREEN_WIDTH / 2.0f;
        camera->lasty       = (float) WINDOW_SCREEN_HEIGHT / 2.0f;
        camera_update_vectors(camera);
}

void
camera_get_view(struct Camera* camera, mat4 dest)
{
        vec3 tmp;
        glm_vec3_add(camera->position, camera->front, tmp);
        glm_lookat(camera->position, tmp, camera->up, dest);
}

void
camera_get_projection(struct Camera* camera, mat4 dest)
{
        glm_perspective(glm_rad(CAMERA_FOV),
                        WINDOW_SCREEN_WIDTH / WINDOW_SCREEN_HEIGHT,
                        0.1f,
                        CAMERA_RENDER_DISTANCE,
                        dest);
}

void
camera_proccess_keyboard(struct Camera* camera, enum CameraMovement direction, float deltatime)
{
        float speed = camera->speed * deltatime;
        vec3  tmp;

        switch (direction)
        {
        case CAM_MOVE_FORWARD:
                glm_vec3_muladds(camera->front, speed, camera->position);
                break;
        case CAM_MOVE_BACKWARD:
                glm_vec3_scale(camera->front, speed, tmp);
                glm_vec3_sub(camera->position, tmp, camera->position);
                break;
        case CAM_MOVE_LEFT:
                glm_vec3_scale(camera->right, speed, tmp);
                glm_vec3_sub(camera->position, tmp, camera->position);
                break;
        case CAM_MOVE_RIGHT:
                glm_vec3_muladds(camera->right, speed, camera->position);
                break;
        case CAM_MOVE_UP:
                glm_vec3_muladds(camera->up, speed, camera->position);
                break;
        case CAM_MOVE_DOWN:
                glm_vec3_scale(camera->up, speed, tmp);
                glm_vec3_sub(camera->position, tmp, camera->position);
                break;
        }
}

void
camera_proccess_mouse(struct Camera* camera, float xoff, float yoff)
{
        if (camera->firstmouse)
        {
                camera->lastx      = xoff;
                camera->lasty      = yoff;
                camera->firstmouse = false;
        }

        float x       = xoff - camera->lastx;
        float y       = camera->lasty - yoff;
        camera->lastx = xoff;
        camera->lasty = yoff;

        x *= camera->sensitivity;
        y *= camera->sensitivity;

        camera->yaw += x;
        camera->pitch += y;

        if (camera->pitch > 89.0f)
                camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
                camera->pitch = -89.0f;

        camera_update_vectors(camera);
}

internal void
camera_update_vectors(struct Camera* camera)
{
        vec3  aux;
        float x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
        float y = sin(glm_rad(camera->pitch));
        float z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));

        glm_vec3_copy((vec3){x, y, z}, aux);
        glm_normalize_to(aux, camera->front);

        glm_vec3_crossn(camera->front, camera->world_up, camera->right);
        glm_vec3_crossn(camera->right, camera->front, camera->up);
}
