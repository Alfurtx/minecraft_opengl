#include "../state.h"
#include "../utils/utils.h"
#include "chunk.h"
#include "world.h"

#define offset(pos) ((uint) (pos[0] + pos[1] * CHUNK_SIZE + pos[2] * CHUNK_SIZE * CHUNK_SIZE))
#define foreach_block(_i, _j, _k)                        \
        for (uint _i = 0; _i < CHUNK_SIZE; _i++)         \
                for (uint _j = 0; _j < CHUNK_SIZE; _j++) \
                        for (uint _k = 0; _k < CHUNK_SIZE; _k++)

internal void        get_neighbor_chunks(struct Chunk* chunk, vec3 position, struct Chunk* dest[2]);
internal inline bool block_on_border(struct Chunk* chunk, vec3 position);

void
chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position)
{
        memset(chunk, 0, sizeof(struct Chunk));
        chunk->world  = world;
        chunk->blocks = calloc(BLOCK_COUNT, sizeof(uint));
        glm_vec3_copy(world_position, chunk->position);
        glm_vec3_scale(world_position, CHUNK_SIZE, chunk->offset);
        chunk->remesh = true;
        mesh_init(&chunk->mesh);
}

void
chunk_update(struct Chunk* chunk)
{
}

void
chunk_destroy(struct Chunk* chunk)
{
        free(chunk->blocks);
        mesh_destroy(&chunk->mesh);
}

void
chunk_generate_mesh(struct Chunk* chunk)
{
        mesh_prepare(&chunk->mesh);

        foreach_block(i, j, k)
        {
                vec3 block_pos, world_pos;
                glm_vec3_copy((vec3){i, j, k}, block_pos);
                glm_vec3_add((vec3){i, j, k}, chunk->offset, world_pos);

                uint block = chunk->blocks[offset(block_pos)];

                if (block != 0)
                {
                        struct Block current, neighbor;

                        current = BLOCKS[block];

                        for (enum Direction direction = 0; direction < 6; direction++)
                        {
                                vec3 dirvec;
                                glm_vec3_copy(DIRECTION_VEC[direction], dirvec);

                                vec3 localvec, worldvec;
                                glm_vec3_add(block_pos, dirvec, localvec);
                                glm_vec3_add(world_pos, dirvec, worldvec);

                                if (block_on_border(chunk, localvec))
                                        neighbor = BLOCKS[chunk->blocks[offset(localvec)]];
                                else
                                        neighbor = BLOCKS[world_get_block(chunk->world, worldvec)];

                                if (!neighbor.active)
                                {
                                        vec2 texture_location;
                                        current.get_texture_location(direction, texture_location);
                                        mesh_add_face(&chunk->mesh, localvec, texture_location, direction);
                                }
                        }
                }
        }

        mesh_finalize(&chunk->mesh);
}

void
chunk_render(struct Chunk* chunk)
{
        if (chunk->remesh && chunk->world->mesh_queue.count < chunk->world->mesh_queue.max)
        {
                chunk_generate_mesh(chunk);
                chunk->remesh = false;
                chunk->world->mesh_queue.count++;
        }

        renderer_set_type(&state.renderer, RENDERER_CHUNK);
        mat4 view       = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        mat4 model      = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, chunk->offset);
        camera_get_view(state.renderer.current_camera, view);
        camera_get_projection(state.renderer.current_camera, projection);
        shader_set_uniform_mat4(state.renderer.current_shader, "projection", 1, GL_FALSE, projection[0]);
        shader_set_uniform_mat4(state.renderer.current_shader, "view", 1, GL_FALSE, view[0]);
        shader_set_uniform_mat4(state.renderer.current_shader, "model", 1, GL_FALSE, model[0]);
        mesh_render(&chunk->mesh);
}

void
chunk_create_map(struct Chunk* chunk)
{
        foreach_block(i, j, k) { chunk_set_block(chunk, (vec3){i, j, k}, BLOCK_GRASS); }
}

uint
chunk_get_block(struct Chunk* chunk, vec3 position)
{
        return chunk_contains_block(chunk, position) ? chunk->blocks[offset(position)] : 0;
}

void
chunk_set_block(struct Chunk* chunk, vec3 position, enum BlockType type)
{
        if (chunk_contains_block(chunk, position))
        {
                struct Chunk* neighbors[2]      = {0};
                chunk->blocks[offset(position)] = type;
                chunk->remesh                   = true;

                if (block_on_border(chunk, position))
                {
                        get_neighbor_chunks(chunk, position, neighbors);
                        for (uint i = 0; i < 2; i++)
                                if (neighbors[i])
                                        neighbors[i]->remesh = true;
                }
        }
}

bool
chunk_contains_block(struct Chunk* chunk, vec3 position)
{
        return position[0] >= 0 && position[1] >= 0 && position[2] >= 0 && position[0] < CHUNK_SIZE &&
               position[1] < CHUNK_SIZE && position[2] < CHUNK_SIZE;
}

internal void
get_neighbor_chunks(struct Chunk* chunk, vec3 position, struct Chunk* dest[2])
{
        uint i = 0;
        vec3 aux;

        if (position[0] == 0)
        {
                glm_vec3_add(chunk->offset, (vec3){-1, 0, 0}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }

        if (position[2] == 0)
        {
                glm_vec3_add(chunk->offset, (vec3){0, 0, -1}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }

        if (position[0] == CHUNK_SIZE - 1)
        {
                glm_vec3_add(chunk->offset, (vec3){1, 0, 0}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }

        if (position[2] == CHUNK_SIZE - 1)
        {
                glm_vec3_add(chunk->offset, (vec3){0, 0, 1}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }
}

internal inline bool
block_on_border(struct Chunk* chunk, vec3 position)
{
        return position[0] == 0 || position[2] == 0 || position[0] == CHUNK_SIZE - 1 || position[2] == CHUNK_SIZE - 1;
}
