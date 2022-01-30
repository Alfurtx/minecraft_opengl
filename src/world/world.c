#include "world.h"

internal uint worldoffset(uint a, uint b);
internal uint chunkoffset(vec3 vec);
internal uint chunkoffset3d(uint a, uint b, uint c);

void
world_init(struct World* world, struct Renderer* renderer)
{
        world->renderer = renderer;
        for (uint i = 0; i < 16; i++)
                for (uint j = 0; j < 16; j++)
                        chunk_init(&world->chunks[i * 16 + j], renderer, (vec3){j, 0, i});

        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_prepare_render(&world->chunks[i]);
}

void
world_destroy(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_destroy(&world->chunks[i]);
}

void
world_update(struct World* world)
{
}

void
world_render(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_render(&world->chunks[i]);
}

// TODO(fonsi): debug
struct Block
world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position)
{
        struct Chunk* chunk;
        for (uint i = 0; i < 3; i++)
        {
                if (i != 1)
                {
                        // NOTE(fonsi): ya que el bloque esta fuera de coordenada se obtiene el bloque al limite
                        // del siguiente chunk o se devuelve el bloque default si esta en el limite de los
                        // chunks
                        if (chunk_block_position[i] <= -1)
                        {
                                vec3 aux = GLM_VEC3_ZERO_INIT;
                                aux[i]   = -1.0f;
                                glm_vec3_add(chunk_world_position, aux, aux);
                                chunk = world_get_chunk(world, aux);

                                if (chunk)
                                {
                                        glm_vec3_copy(chunk_block_position, aux);
                                        aux[i] = 15.0f;
                                        return (chunk->blocks[chunkoffset(aux)]);
                                }

                                return BLOCK_DEFAULT;
                        }
                        else if (chunk_block_position[i] >= 16)
                        {
                                vec3 aux = GLM_VEC3_ZERO_INIT;
                                aux[i]   = 1.0f;
                                glm_vec3_add(chunk_world_position, aux, aux);
                                chunk = world_get_chunk(world, aux);

                                if (chunk)
                                {
                                        glm_vec3_copy(chunk_block_position, aux);
                                        aux[i] = 0.0f;
                                        return (chunk->blocks[chunkoffset(aux)]);
                                }

                                return BLOCK_DEFAULT;
                        }
                }
        }

        chunk = world_get_chunk(world, chunk_world_position);
        return (chunk->blocks[chunkoffset(chunk_block_position)]);
}

// TODO(fonsi): debug
struct Chunk*
world_get_chunk(struct World* world, vec3 chunk_world_position)
{
        for (uint i = 0; i < 3; i++)
        {
                if (chunk_world_position[i] <= -1)
                {
                        vec3 aux = GLM_VEC3_ZERO_INIT;
                        aux[i]   = 1.0f;
                        glm_vec3_add(chunk_world_position, aux, aux);
                        return &world->chunks[worldoffset(aux[0], aux[2])];
                }
                else if (chunk_world_position[i] >= 16)
                {
                        vec3 aux = GLM_VEC3_ZERO_INIT;
                        aux[i]   = -1.0f;
                        glm_vec3_add(chunk_world_position, aux, aux);
                        return &world->chunks[worldoffset(aux[0], aux[2])];
                }
        }

        return NULL;
}

internal uint
worldoffset(uint a, uint b)
{
        return (a * 16 + b);
}

internal uint
chunkoffset(vec3 vec)
{
        return (vec[0] + CHUNK_SIZE_X * vec[1] + CHUNK_SIZE_X * CHUNK_SIZE_Y * vec[2]);
}

internal uint
chunkoffset3d(uint a, uint b, uint c)
{
        return (a + CHUNK_SIZE_X * b + CHUNK_SIZE_X * CHUNK_SIZE_Y * c);
}
