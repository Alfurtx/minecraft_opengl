#include "../utils/types.h"
#include "camera.h"
#include "gfx.h"

const float CAMERA_RENDER_DISTANCE = 512.0f;
const float CAMERA_YAW             = -90.0f;
const float CAMERA_PITCH           = 0.0f;
const float CAMERA_SPEED           = 2.5f;
const float CAMERA_SENSITIVITY     = 0.1f;
const float CAMERA_FOV             = 45.0f;

internal void camera_update_vectors(struct Camera* camera);

void
camera_init(struct Camera* camera)
{
        glm_vec3_copy(GLM_VEC3_ZERO, camera->front);
        glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->world_up);
        glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
        camera->yaw         = CAMERA_YAW;
        camera->pitch       = CAMERA_PITCH;
        camera->speed       = CAMERA_SPEED;
        camera->sensitivity = CAMERA_SENSITIVITY;
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
        camera->pitch = glm_clamp(camera->pitch, -89.0f, 89.0f);
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
