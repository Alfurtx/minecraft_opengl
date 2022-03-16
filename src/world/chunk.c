#include "../state.h"
#include "../utils/utils.h"
#include "chunk.h"
#include "world.h"

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
chunk_generate_mesh(struct Chunk* chunk)
{
}

void
chunk_render(struct Chunk* chunk)
{
        renderer_set_type(&state.renderer, RENDERER_CHUNK);
        mat4 view       = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        mat4 model      = GLM_MAT4_IDENTITY_INIT;
        // glm_translate(model, chunk->offset);
        camera_get_view(state.renderer.current_camera, view);
        camera_get_projection(state.renderer.current_camera, projection);
        shader_set_uniform_mat4(state.renderer.current_shader, "projection", 1, GL_FALSE, projection[0]);
        shader_set_uniform_mat4(state.renderer.current_shader, "view", 1, GL_FALSE, view[0]);
        shader_set_uniform_mat4(state.renderer.current_shader, "model", 1, GL_FALSE, model[0]);
        // mesh_render(&chunk->mesh);
}

void
chunk_create_map(struct Chunk* chunk)
{
}
