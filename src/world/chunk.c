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

#define CHUNK_GETINDEX(pos) (pos[0] * CHUNK_SIZE_Y * CHUNK_SIZE_Z + pos[1] * CHUNK_SIZE_Z + pos[0])
#define CHUNK_FOREACH(x, y, z)                          \
        for (uint x = 0; x < CHUNK_SIZE_X; x++)         \
                for (uint y = 0; y < CHUNK_SIZE_Y; y++) \
                        for (uint z = 0; z < CHUNK_SIZE_Z; z++)

internal inline bool chunk_pos_in_chunk(vec3 position);
internal inline bool chunk_pos_on_border(vec3 position);
internal void        chunk_set_block(struct Chunk* chunk, vec3 position, uint type);
internal void        chunk_get_neighbor_chunks(struct Chunk* chunk, vec3 position, struct Chunk* dest[2]);

void
chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position)
{
        memset(chunk, 0, sizeof(struct Chunk));
        chunk->world = world;
        mesh_init(&chunk->mesh);
        glm_vec3_copy(world_position, chunk->position);
        glm_vec3_scale(chunk->position, CHUNK_SIZE, chunk->offset);
        chunk->blocks = calloc(1, sizeof(uint) * CHUNK_BLOCK_COUNT);
}

void
chunk_update(struct Chunk* chunk)
{
}

void
chunk_destroy(struct Chunk* chunk)
{
        mesh_destroy(&chunk->mesh);
        free(chunk->blocks);
}

void
chunk_generate_mesh(struct Chunk* chunk)
{
        mesh_prepare(&chunk->mesh);

        CHUNK_FOREACH(x, y, z)
        {
                vec3 pos, world_pos;
                glm_vec3_copy((vec3){x, y, z}, pos);
                glm_vec3_add(pos, chunk->offset, world_pos);
                uint         current = chunk->blocks[(uint) CHUNK_GETINDEX(pos)];
                struct Block block   = BLOCKS[current];

                if (current)
                {
                        for (enum Direction d = 0; d < 6; d++)
                        {
                                vec2 texture_position;
                                block.get_texture_location(d, texture_position);

                                vec3 neighbor, world_neighbor;
                                glm_vec3_add(pos, DIRECTION_VEC[d], neighbor);
                                glm_vec3_add(world_pos, DIRECTION_VEC[d], world_neighbor);

                                struct Block neighbor_block;

                                if (chunk_pos_in_chunk(neighbor))
                                        neighbor_block = BLOCKS[chunk_get_block(chunk, neighbor)];
                                else
                                        neighbor_block = BLOCKS[world_get_block(chunk->world, world_neighbor)];

                                if (!neighbor_block.active)
                                        mesh_add_face(&chunk->mesh, pos, texture_position, d);
                        }
                }
        }

        mesh_finalize(&chunk->mesh);
}

void
chunk_render(struct Chunk* chunk)
{
        if (chunk->flags.remesh && chunk->world->throttles.mesh.count < chunk->world->throttles.mesh.max)
        {
                chunk_generate_mesh(chunk);
                chunk->flags.remesh = false;
                chunk->world->throttles.mesh.count++;
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
        for (uint i = 0; i < CHUNK_SIZE_X; i++)
                for (uint j = 0; j < 4; j++)
                        for (uint k = 0; k < CHUNK_SIZE_Z; k++)
                                chunk_set_block(chunk, (vec3){i, j, k}, BLOCK_GRASS);
}

internal inline bool
chunk_pos_in_chunk(vec3 position)
{
        return position[0] >= 0 && position[1] >= 0 && position[2] >= 0 && position[0] < CHUNK_SIZE_X &&
               position[1] < CHUNK_SIZE_Y && position[2] < CHUNK_SIZE_Z;
}

internal inline bool
chunk_pos_on_border(vec3 position)
{
        return position[0] == 0 && position[2] == 0 && position[0] == (CHUNK_SIZE_X - 1) &&
               position[2] == (CHUNK_SIZE_Z - 1);
}

uint
chunk_get_block(struct Chunk* chunk, vec3 position)
{
        return chunk->blocks[(uint) CHUNK_GETINDEX(position)];
}

internal void
chunk_set_block(struct Chunk* chunk, vec3 position, uint type)
{
        chunk->blocks[(uint) CHUNK_GETINDEX(position)] = type;
        chunk->flags.remesh                            = true;

        if (chunk_pos_in_chunk(position))
        {
                struct Chunk* aux[2] = {NULL, NULL};
                chunk_get_neighbor_chunks(chunk, position, aux);

                for (uint i = 0; i < 2; i++)
                {
                        if (aux[i])
                                aux[i]->flags.remesh = true;
                }
        }
}

internal void
chunk_get_neighbor_chunks(struct Chunk* chunk, vec3 position, struct Chunk* dest[2])
{
        uint i = 0;

        if (position[0] == 0)
        {
                vec3 aux;
                glm_vec3_add(chunk->offset, (vec3){-1, 0, 0}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }

        if (position[2] == 0)
        {
                vec3 aux;
                glm_vec3_add(chunk->offset, (vec3){0, 0, -1}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }

        if (position[0] == CHUNK_SIZE_X - 1)
        {
                vec3 aux;
                glm_vec3_add(chunk->offset, (vec3){1, 0, 0}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }

        if (position[2] == CHUNK_SIZE_Z - 1)
        {
                vec3 aux;
                glm_vec3_add(chunk->offset, (vec3){0, 0, 1}, aux);
                dest[i++] = world_get_chunk(chunk->world, aux);
        }
}
