#include "world.h"

struct block_index_value_offset
{
        float value;
        uint  index;
};
internal struct block_index_value_offset get_block_index_value_offset(vec3 block_position);

internal uint worldoffset(vec3 chunk_position);
internal uint chunkoffset(vec3 vec);
internal uint chunkoffset3d(uint a, uint b, uint c);

// TODO(fonsi): Actualizar estos metodos para adecuarlos a la generacion infinita del mundo, asi como a chunks con chunk
// position negativas
internal bool world_chunk_in_bounds(struct World* world, vec3 chunk_world_position);
internal bool world_block_in_chunk_bounds(struct World* world, vec3 chunk_block_position);

#define FOR_EACH_CHUNK(ptr, func)                    \
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++) \
                func(ptr->chunks[i]);

#define FOR_EACH_CHUNK_XZ(i, j)                      \
        for (uint i = 0; i < WORLD_STATIC_SIDE; i++) \
                for (uint j = 0; j < WORLD_STATIC_SIDE; j++)

/*
 * NOTE(fonsi): Recordar cambiar de orden el X y Z a la hora de pasar los valores a chunk_position
 *
 * =>
 *
 * FOR_EACH_CHUNK_XZ(i, j) { chunk_init(renderer, (vec3){j, 0, i}); }
 *
 */

const vec3 WORLD_CHUNK_SURROUNDINGS[] = {
    { 1.0f, 0.0f,  0.0f}, // +x
    {-1.0f, 0.0f,  0.0f}, // -x
    { 0.0f, 0.0f,  1.0f}, // +z
    { 0.0f, 0.0f, -1.0f}, // -z
    { 1.0f, 0.0f, -1.0f},
    {-1.0f, 0.0f,  1.0f},
    { 1.0f, 0.0f,  1.0f},
    {-1.0f, 0.0f, -1.0f},
};

internal void world_load_surrounding_chunks(struct World* world);

void
world_init(struct World* world, struct Renderer* renderer)
{
        world->renderer = renderer;
        world->chunks   = malloc(sizeof *world->chunks * WORLD_CHUNK_COUNT);
        glm_vec3_zero(world->chunk_origin);
}

void
world_destroy(struct World* world)
{
}

void
world_update(struct World* world)
{
}

void
world_render(struct World* world)
{
}

struct Block
world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position)
{
        if (world_block_in_chunk_bounds(world, chunk_block_position))
                return world->chunks[worldoffset(chunk_world_position)]->blocks[chunkoffset(chunk_block_position)];
        else
        {
                // calcular el chunk al que le pertenece el bloque
                struct block_index_value_offset offset_data = get_block_index_value_offset(chunk_block_position);
                if (offset_data.index == 1)
                        return BLOCK_DEFAULT;

                vec3 aux;
                vec3 neighbor_block;

                glm_vec3_copy(chunk_block_position, neighbor_block);
                glm_vec3_copy(chunk_world_position, aux);

                if (offset_data.value >= 16)
                {
                        aux[offset_data.index] += 1.0f;
                        neighbor_block[offset_data.index] = 0.0f;
                }
                else
                {
                        aux[offset_data.index] -= 1.0f;
                        neighbor_block[offset_data.index] = 15.0f;
                }

                struct Chunk* chunk = world_get_chunk(world, aux);
                if (!chunk)
                        return BLOCK_DEFAULT;
                else
                        return chunk->blocks[chunkoffset(neighbor_block)];
        }

        return BLOCK_DEFAULT;
}

struct Chunk*
world_get_chunk(struct World* world, vec3 chunk_world_position)
{
        if (!world_chunk_in_bounds(world, chunk_world_position))
                return NULL;
        return world->chunks[worldoffset(chunk_world_position)];
}

internal uint
worldoffset(vec3 chunk_position)
{
        return ((uint) chunk_position[2] * WORLD_STATIC_SIDE + (uint) chunk_position[0]);
}

internal uint
chunkoffset(vec3 vec)
{
        return ((uint) vec[0] + CHUNK_SIZE_X * (uint) vec[1] + CHUNK_SIZE_X * CHUNK_SIZE_Y * (uint) vec[2]);
}

internal uint
chunkoffset3d(uint a, uint b, uint c)
{
        return (a + CHUNK_SIZE_X * b + CHUNK_SIZE_X * CHUNK_SIZE_Y * c);
}

internal bool
world_chunk_in_bounds(struct World* world, vec3 chunk_world_position)
{
        return (chunk_world_position[0] >= 0 && chunk_world_position[2] >= 0 &&
                chunk_world_position[0] < WORLD_STATIC_SIDE && chunk_world_position[2] < WORLD_STATIC_SIDE);
}

internal bool
world_block_in_chunk_bounds(struct World* world, vec3 chunk_block_position)
{
        return (chunk_block_position[0] >= 0 && chunk_block_position[1] >= 0 && chunk_block_position[2] >= 0 &&
                chunk_block_position[0] < CHUNK_SIZE_X && chunk_block_position[1] < CHUNK_SIZE_Y &&
                chunk_block_position[2] < CHUNK_SIZE_Z);
}

internal struct block_index_value_offset
get_block_index_value_offset(vec3 block_position)
{
        struct block_index_value_offset res = {0};
        for (uint i = 0; i < 3; i++)
        {
                if (block_position[i] <= -1 || (block_position[i] >= 16 && i != 1) || block_position[i] >= 256)
                {
                        res.index = i;
                        res.value = block_position[i];
                        return (res);
                }
        }

        res.index = -1;
        res.value = 0;
        return res;
}

internal void
world_load_surrounding_chunks(struct World* world)
{
}
