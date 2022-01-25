#include "../utils/types.h"
#include "camera.h"
#include "gfx.h"

internal void camera_update_vectors(struct Camera* camera);

struct Camera
camera_create()
{
        struct Camera res;

        glm_vec3_copy(GLM_VEC3_ZERO, res.front);
        glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, res.world_up);
        glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, res.front);
        res.yaw         = CAMERA_YAW;
        res.pitch       = CAMERA_PITCH;
        res.speed       = CAMERA_SPEED;
        res.sensitivity = CAMERA_SENSITIVITY;
        camera_update_vectors(&res);

        return (res);
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
        glm_perspective(glm_rad(CAMERA_FOV), WINDOW_SCREEN_WIDTH / WINDOW_SCREEN_HEIGHT, 0.1f, 1000.0f, dest);
}

void
camera_process_keyboard(struct Camera* camera, enum CameraMovement direction, float deltatime)
{
        // TODO(fonsi): implementar keyboard con window keyboard callback
}

void
camera_process_mouse(struct Camera* camera, float x_off, float y_off)
{
        // TODO(fonsi): mejorar implementación con window mouse callback

        x_off *= camera->sensitivity;
        y_off *= camera->sensitivity;
        camera->yaw += x_off;
        camera->pitch += y_off;
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
