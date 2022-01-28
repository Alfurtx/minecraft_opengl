#ifndef RENDERER_H_
#define RENDERER_H_

#include "camera.h"
#include "shader.h"
#include "texture.h"

// TODO(fonsi): renderer for sky and ui
enum RendererType
{
        RENDERER_CHUNK = 0,
        RENDERER_TYPE_COUNT,
};

struct Renderer
{
        struct Shader  shaders[RENDERER_TYPE_COUNT];
        struct Texture textures[RENDERER_TYPE_COUNT];
        struct Camera  cameras[RENDERER_TYPE_COUNT];

        struct Shader*  current_shader;
        struct Texture* current_texture;
        struct Camera*  current_camera;
};

extern void renderer_init(struct Renderer* renderer);
extern void renderer_destroy(struct Renderer* renderer); // TODO(fonsi): comprobar que de verdad necesito esto
extern void renderer_set_type(struct Renderer* renderer, enum RendererType type);

#endif // RENDERER_H_
