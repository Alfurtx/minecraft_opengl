#include "../state.h"
#include "../utils/utils.h"
#include "chunk.h"
#include "world.h"

/*
** TODO(fonsi): Lista de problemas a resolver ->
** 1. Mejorar la generacion de mundo, multiples noise maps para hacer biomas y demás. Utilizar Perlin en vez del otro.
** 2. Mejorar la generacion del mesh. Esto tendra que ver con utilizar buffers globales para almacenar los datos del
*mesh.
** 3. Mejorar la comprobacion de caras de los bloques para que se haga lo mas rapidamente posible.
** 4. Implementar de alguna manera los INDEX buffer objects en todo este lio
*/

// NOTE(fonsi): NO hacer nada de multithreading, hay que resolver esto sin meter mas hilos, eso solo lo complica todo
// mas de la cuenta.

#define CHUNK_FOREACH(x, y, z)                          \
        for (uint x = 0; x < CHUNK_SIZE_X; x++)         \
                for (uint y = 0; y < CHUNK_SIZE_Y; y++) \
                        for (uint z = 0; z < CHUNK_SIZE_Z; z++)

void
chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position)
{
}

void
chunk_update(struct Chunk* chunk)
{
}

void
chunk_destroy(struct Chunk* chunk)
{
}

void
chunk_prepare_render(struct Chunk* chunk)
{
}

void
chunk_render(struct Chunk* chunk)
{
        renderer_set_type(chunk->renderer, RENDERER_CHUNK);
        mat4 view       = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        mat4 model      = GLM_MAT4_IDENTITY_INIT;
        glm_translate_x(model, chunk->world_offset[0]);
        glm_translate_y(model, chunk->world_offset[1]);
        glm_translate_z(model, chunk->world_offset[2]);
        camera_get_view(chunk->renderer->current_camera, view);
        camera_get_projection(chunk->renderer->current_camera, projection);
        shader_set_uniform_mat4(chunk->renderer->current_shader, "projection", 1, GL_FALSE, projection[0]);
        shader_set_uniform_mat4(chunk->renderer->current_shader, "view", 1, GL_FALSE, view[0]);
        shader_set_uniform_mat4(chunk->renderer->current_shader, "model", 1, GL_FALSE, model[0]);
        mesh_render(&chunk->mesh);
}
