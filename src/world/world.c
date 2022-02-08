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
internal bool check_player_in_chunk_origin(struct World* world);
internal void free_unloaded_chunks(struct World* world);

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

#define WORLD_CHUNK_SURROUNDINGS_COUNT 9
vec3 WORLD_CHUNK_SURROUNDINGS[] = {
    { 0.0f, 0.0f,  0.0f},
    { 1.0f, 0.0f,  0.0f}, // +x
    {-1.0f, 0.0f,  0.0f}, // -x
    { 0.0f, 0.0f,  1.0f}, // +z
    { 0.0f, 0.0f, -1.0f}, // -z
    { 1.0f, 0.0f, -1.0f},
    {-1.0f, 0.0f,  1.0f},
    { 1.0f, 0.0f,  1.0f},
    {-1.0f, 0.0f, -1.0f},
};

internal void world_init_surrounding_chunks(struct World* world);
internal void world_set_unload_surrounding_chunks(struct World* world);
internal void set_new_chunk_origin(struct World* world, vec3 dest);

void
world_init(struct World* world, struct Renderer* renderer)
{
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
}

struct Chunk*
world_get_chunk(struct World* world, vec3 chunk_world_position)
{
}
