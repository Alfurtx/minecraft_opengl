#include "renderer.h"

void
renderer_init(struct Renderer* renderer)
{
        for (uint i = 0; i < RENDERER_TYPE_COUNT; i++)
        {
                switch (i)
                {
                case RENDERER_CHUNK:
                #ifdef __APPLE__
                        renderer->shaders[i]  = shader_create("/Usera/alfonso/Desktop/Proyectos\ Personales/minecraft_opengl/res/shaders/chunk.vert", "/Usera/alfonso/Desktop/Proyectos\ Personales/minecraft_opengl/res/shaders/chunk.frag");
                        renderer->textures[i] = texture_create("/Usera/alfonso/Desktop/Proyectos\ Personales/minecraft_opengl/res/textures/blockatlas.png");
                #else
                        renderer->shaders[i]  = shader_create("/home/fonsi/proyectos/minecraft_opengl/res/shaders/chunk.vert", "/home/fonsi/proyectos/minecraft_opengl/res/shaders/chunk.frag");
                        renderer->textures[i] = texture_create("/home/fonsi/proyectos/minecraft_opengl/res/textures/blockatlas.png");
                #endif
                        break;
                }
                camera_init(&renderer->cameras[i]);
        }
}

// NOTE(fonsi): comprobar que de verdad necesito esto
void
renderer_destroy(struct Renderer* renderer)
{
}

void
renderer_set_type(struct Renderer* renderer, enum RendererType type)
{
        renderer->current_camera  = &renderer->cameras[type];
        renderer->current_shader  = &renderer->shaders[type];
        renderer->current_texture = &renderer->textures[type];

        shader_use(renderer->current_shader);
        texture_bind(renderer->current_texture);
}
