#include "world.h"

// TODO(fonsi): eliminar si al final no es usado
#define WORLD_CHUNK_SURROUNDINGS_COUNT 9
vec3 WORLD_CHUNK_SURROUNDINGS[] = {
    { 0.0f, 0.0f,  0.0f},
    { 1.0f, 0.0f,  0.0f},
    {-1.0f, 0.0f,  0.0f},
    { 0.0f, 0.0f,  1.0f},
    { 0.0f, 0.0f, -1.0f},
    { 1.0f, 0.0f, -1.0f},
    {-1.0f, 0.0f,  1.0f},
    { 1.0f, 0.0f,  1.0f},
    {-1.0f, 0.0f, -1.0f},
};

#define BLOCKOFFSET(vec) ((uint) vec[0] + CHUNK_SIZE_X * (uint) vec[1] + CHUNK_SIZE_X * CHUNK_SIZE_Y * (uint) vec[2])
#define CHUNKOFFSET(vec) ((uint) vec[0] + WORLD_CHUNK_SIDE * (uint) vec[2])

struct value_index
{
        float value;
        int   index;
};
// NOTE(fonsi): si block_position no tiene ninguna coordenada fuera del rango de un chunk (ej. -1, 16 o 256), devuelve
// index = -1
internal struct value_index get_block_index_value_coord(vec3 block_position);

internal bool is_chunk_in_bounds(struct World* world, vec3 chunk_world_position);
internal bool is_block_in_chunk_bounds(struct World* world, vec3 chunk_block_position);
internal void get_chunkpos_from_position(vec3 position, vec3 dest);
internal bool check_player_in_chunk_origin(struct World* world);

/*
 * NOTE(fonsi): Recordar cambiar de orden el X y Z a la hora de pasar los valores a chunk_position
 * =>
 * FOR_EACH_CHUNK_XZ(i, j) { chunk_init(renderer, (vec3){j, 0, i}); }
 */

void
world_init(struct World* world, struct Renderer* renderer)
{
        world->renderer = renderer;
        world->chunks   = malloc(WORLD_CHUNK_COUNT * sizeof *world->chunks);
        get_chunkpos_from_position(world->renderer->current_camera->position, world->chunk_origin);

        // ir inicializando cada chunk, basandose en la direccion desde el chunk_origin
        int k = 0;
        for (int i = 2; i >= -2; i--)
                for (int j = 2; j >= -2; j--)
                {
                        vec3 aux;
                        glm_vec3_copy((vec3){i, 0, j}, aux);
                        glm_vec3_add(world->chunk_origin, aux, aux);
                        world->chunks[k] = chunk_init(renderer, aux);
                        k++;
                }
}

void
world_destroy(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_destroy(world->chunks[i]);
        free(world->chunks);
}

// PERFORMANCE(fonsi): esta funcion va demasiado lento
void
world_update(struct World* world)
{
        if (!check_player_in_chunk_origin(world))
        {
                vec3 new_chunk_origin;
                get_chunkpos_from_position(world->renderer->current_camera->position, new_chunk_origin);
                glm_vec3_copy(new_chunk_origin, world->chunk_origin);

                // por cada chunk, comprobar que debe mantenerse cargado, sino flagearlo para su posterior free
                for (uint k = 0; k < WORLD_CHUNK_COUNT; k++)
                {
                        world->chunks[k]->loaded = false;
                        for (int i = 2; i >= -2; i--)
                                for (int j = 2; j >= -2; j--)
                                {
                                        vec3 aux;
                                        glm_vec3_copy((vec3){i, 0, j}, aux);
                                        glm_vec3_add(world->chunk_origin, aux, aux);
                                        if (glm_vec3_eqv(aux, world->chunks[k]->world_position))
                                                world->chunks[k]->loaded = true;
                                }
                }

                // free los chunks que no estan loaded
                for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                        if (!world->chunks[i]->loaded)
                                chunk_destroy(world->chunks[i]);

                // inicializar los espacios vacios a nuevos chunks
                for (int i = 2; i >= -2; i--)
                        for (int j = 2; j >= -2; j--)
                        {
                                vec3 aux;
                                glm_vec3_copy((vec3){i, 0, j}, aux);
                                glm_vec3_add(world->chunk_origin, aux, aux);
                                struct Chunk* chunk = world_get_chunk(world, aux);
                                if (!chunk)
                                        chunk = chunk_init(world->renderer, aux);
                        }
        }
}

// PERFORMANCE(fonsi): rendimiento pesimo con chunk_prepare_render -> o hago hilos o veo como optimizar esa funcion
void
world_render(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                if(!world->chunks[i]->prepared)
                {
                        chunk_prepare_render(world->chunks[i]);
                        world->chunks[i]->prepared = true;
                }

        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_render(world->chunks[i]);
}

struct Block
world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position)
{
        struct value_index aux = get_block_index_value_coord(chunk_block_position);
        struct Chunk*      chunk;
        vec3               new_chunk_position;
        vec3               new_block_position;

        glm_vec3_copy(chunk_world_position, new_chunk_position);
        glm_vec3_copy(chunk_block_position, new_block_position);

        // modificar valor de chunk_world_position si el bloque esta fuera de chunk
        if (aux.index != -1)
        {
                if (aux.index == 1)
                        return BLOCK_DEFAULT;
                if (aux.value == -1)
                {
                        new_chunk_position[aux.index]--;
                        new_block_position[aux.index] = 15;
                }
                if (aux.value == 16)
                {
                        new_chunk_position[aux.index]++;
                        new_block_position[aux.index] = 0;
                }
        }

        // conseguir el chunk del bloque
        chunk = world_get_chunk(world, new_chunk_position);

        if (!chunk)
                return BLOCK_DEFAULT;
        else
        {
                return chunk->blocks[BLOCKOFFSET(new_block_position)];
        }
}

struct Chunk*
world_get_chunk(struct World* world, vec3 chunk_world_position)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                if (glm_vec3_eqv(world->chunks[i]->world_position, chunk_world_position))
                        return world->chunks[i];
        return NULL;
}

internal void
get_chunkpos_from_position(vec3 position, vec3 dest)
{
        dest[0] = floorf(position[0] / 16.0f);
        dest[1] = 0;
        dest[2] = floorf(position[2] / 16.0f);
}

internal struct value_index
get_block_index_value_coord(vec3 block_position)
{
        struct value_index res = {0, -1};
        for (uint i = 0; i < 3; i++)
        {
                if (block_position[i] == -1 || block_position[i] == 16 || block_position[i] == 256)
                {
                        res.index = i;
                        res.value = block_position[i];
                        return res;
                }
        }
        return res;
}

internal bool
check_player_in_chunk_origin(struct World* world)
{
        vec3 aux = GLM_VEC3_ZERO_INIT;
        get_chunkpos_from_position(world->renderer->current_camera->position, aux);
        return glm_vec3_eqv(world->chunk_origin, aux);
}
