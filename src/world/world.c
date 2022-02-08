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
        // comprobar si camara position es diferente de chunk_origin
        if (!check_player_in_chunk_origin(world))
        {
                // set el nuevo chunk_origin
                set_new_chunk_origin(world, world->chunk_origin);

                // set loaded=false en los chunks fuera del radio de chunk_origin
                world_set_unload_surrounding_chunks(world);

                // free los chunks con loaded=false
                // comprobar que todos los chunks dentro del radio nuevo esten inicializados y sino, inicializarlos
        }
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

internal bool
check_player_in_chunk_origin(struct World* world)
{
        vec3 pos;
        glm_vec3_copy(world->renderer->cameras[RENDERER_CHUNK].position, pos);
        vec3 chunk_min;
        glm_vec3_scale(world->chunk_origin, CHUNK_SIZE_X, chunk_min);
        vec3 chunk_max;
        glm_vec3_adds(chunk_min, CHUNK_SIZE_X, chunk_max);

        return ((pos[0] <= chunk_max[0] && pos[0] >= chunk_min[0]) &&
                (pos[1] <= chunk_max[1] && pos[1] >= chunk_min[1]) &&
                (pos[2] <= chunk_max[2] && pos[2] >= chunk_min[2]));
}

internal void
world_init_surrounding_chunks(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_SURROUNDINGS_COUNT; i++)
                for (uint j = 0; j < WORLD_CHUNK_SURROUNDINGS_COUNT; j++)
                {
                        vec3 aux;
                        glm_vec3_add(WORLD_CHUNK_SURROUNDINGS[i], WORLD_CHUNK_SURROUNDINGS[j], aux);
                        glm_vec3_add(world->chunk_origin, aux, aux);
                        struct Chunk* chunk = world->chunks[worldoffset(aux)];
                        if (!chunk)
                                chunk = chunk_init(world->renderer, aux);
                }
}

// NOTE(fonsi): para cada chunk, una vez actualizado chunk_origin, comprobar que no se encuentra en el nuevo rango de
// chunks deseados. Si no está, marcarlo para hacerle unload
internal void
world_set_unload_surrounding_chunks(struct World* world)
{
        for (uint k = 0; k < WORLD_CHUNK_COUNT; k++)
        {
                bool          should_stay_loaded = false;
                struct Chunk* chunk              = world->chunks[k];

                for (uint i = 0; i < WORLD_CHUNK_SURROUNDINGS_COUNT; i++)
                        for (uint j = 0; j < WORLD_CHUNK_SURROUNDINGS_COUNT; j++)
                        {
                                vec3 aux;
                                glm_vec3_add(WORLD_CHUNK_SURROUNDINGS[i], WORLD_CHUNK_SURROUNDINGS[j], aux);
                                glm_vec3_add(world->chunk_origin, aux, aux);
                                if (glm_vec3_eqv(chunk->world_position, aux))
                                        should_stay_loaded = true;
                        }
                chunk->loaded = should_stay_loaded;
        }
}

internal void
set_new_chunk_origin(struct World* world, vec3 dest)
{
        uint x = world->renderer->current_camera->position[0] / 16;
        uint z = world->renderer->current_camera->position[2] / 16;
        glm_vec3_copy((vec3){x, 0, z}, dest);
}

internal void
free_unloaded_chunks(struct World* world)
{
}
