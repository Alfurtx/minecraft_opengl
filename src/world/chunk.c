#include "../state.h"
#include "chunk.h"
#include "world.h"

// TODO(fonsi): implementar generacion de mundo apropiada
internal void chunk_setup_map(struct Chunk* chunk);
internal uint offset(uint x, uint y, uint z);
internal void offset3d(uint offset, vec3 dest);

#define FOR_EACH_BLOCK(func) for (uint i = 0; i < CHUNK_BLOCK_COUNT; i++) func(&chunk->blocks[i]);

struct Chunk*
chunk_init(struct Renderer* renderer, vec3 world_position)
{
        struct Chunk* chunk = calloc(1, sizeof *chunk);
        mesh_init(&chunk->mesh);
        chunk->loaded   = false;
        chunk->renderer = renderer;
        glm_vec3_copy(world_position, chunk->world_position);
        glm_vec3_scale(world_position, 16.0f, chunk->world_offset);

        for (uint i = 0; i < CHUNK_BLOCK_COUNT; i++)
                block_init(&chunk->blocks[i]);

        chunk_setup_map(chunk);

        return (chunk);
}

void
chunk_update(struct Chunk* chunk)
{
}

void
chunk_destroy(struct Chunk* chunk)
{
        mesh_destroy(&chunk->mesh);
}

void
chunk_prepare_render(struct Chunk* chunk)
{
        for (uint i = 0; i < CHUNK_SIZE_X; i++)
                for (uint j = 0; j < CHUNK_SIZE_Y; j++)
                        for (uint k = 0; k < CHUNK_SIZE_Z; k++)
                        {
                                vec3 block_position;
                                glm_vec3_copy((vec3){i, j, k}, block_position);
                                struct Block current_block = chunk->blocks[offset(i, j, k)];
                                if (current_block.active)
                                {
                                        // ir cara por cara de cada bloque
                                        for (uint d = 0; d < DIRECTION_COUNT; d++)
                                        {
                                                // conseguir el bloque vecino
                                                vec3 aux;
                                                glm_vec3_add(block_position, DIRECTION_VEC[d], aux);
                                                struct Block neighbor_block =
                                                    world_get_block(&state.world, chunk->world_position, aux);

                                                // comprobar que si bloque adyacente no esta activo
                                                if (!neighbor_block.active)
                                                {
                                                        // pasarlo al mesh
                                                        vec2 texture_coords;
                                                        current_block.get_texture_location(d, texture_coords);
                                                        mesh_add_face(&chunk->mesh, block_position, texture_coords, d);
                                                }
                                        }
                                }
                        }
        mesh_prepare_render(&chunk->mesh);
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

internal uint
offset(uint x, uint y, uint z)
{
        return (x + CHUNK_SIZE_X * y + CHUNK_SIZE_X * CHUNK_SIZE_Y * z);
}

internal void
offset3d(uint offset, vec3 dest)
{
        dest[2] = offset / (CHUNK_SIZE_X * CHUNK_SIZE_Y);
        offset -= (dest[2] * CHUNK_SIZE_X * CHUNK_SIZE_Y);
        dest[1] = offset / CHUNK_SIZE_X;
        dest[0] = offset % CHUNK_SIZE_X;
}

internal void
chunk_setup_map(struct Chunk* chunk)
{
        uint index = (uint) chunk->world_position[0] % 8 + (uint) chunk->world_position[2] % 8;
        for (uint i = 0; i < CHUNK_SIZE_X; i++)
                for (uint j = 0; j < CHUNK_SIZE_Z; j++)
                        for (uint k = 0; k < 4; k++)
                                chunk->blocks[offset(i, k, j)] = BLOCKS[index];
}
