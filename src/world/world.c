#include "world.h"

#define WORLD_CHUNK_RENDER_DISTANCE 4
#define WORLD_CHUNK_SURROUNDINGS_COUNT ((WORLD_CHUNK_RENDER_DISTANCE * 2 + 1) * (WORLD_CHUNK_RENDER_DISTANCE * 2 + 1))
vec3 WORLD_CHUNK_SURROUNDINGS[WORLD_CHUNK_SURROUNDINGS_COUNT];

#define WORLD_CHUNK_SIDE_COUNT (WORLD_CHUNK_RENDER_DISTANCE * 2 + 1)
#define WORLD_CHUNK_COUNT WORLD_CHUNK_SURROUNDINGS_COUNT

#define BLOCKOFFSET(vec) ((uint) vec[0] + CHUNK_SIZE_X * (uint) vec[1] + CHUNK_SIZE_X * CHUNK_SIZE_Y * (uint) vec[2])
#define CHUNKOFFSET(vec) ((uint) vec[0] + WORLD_CHUNK_SIDE * (uint) vec[2])
#define FOR_EACH_POSITION(i, j)                                                              \
        for (int i = -WORLD_CHUNK_RENDER_DISTANCE; i < WORLD_CHUNK_RENDER_DISTANCE + 1; i++) \
                for (int j = -WORLD_CHUNK_RENDER_DISTANCE; j < WORLD_CHUNK_RENDER_DISTANCE + 1; j++)

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
internal bool player_in_chunk_origin(struct World* world);
internal void set_border_chunks(struct World* world);

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

        uint index = 0;
        FOR_EACH_POSITION(i, j) { glm_vec3_copy((vec3){i, 0, j}, WORLD_CHUNK_SURROUNDINGS[index++]); }

        // FastNoiseLite init
        // world->noise_state = fnlCreateState();
        // world->noise_state.noise_type       = FNL_NOISE_OPENSIMPLEX2;
        // world->noise_state.octaves          = 16;
        // world->noise_state.lacunarity       = 4.0;
        // world->noise_state.frequency        = 0.01;
        // world->noise_state.fractal_type     = FNL_FRACTAL_FBM;

        // ir inicializando cada chunk, basandose en la direccion desde el chunk_origin
        for (uint i = 0; i < WORLD_CHUNK_SURROUNDINGS_COUNT; i++)
        {
                vec3 aux;
                glm_vec3_add(world->chunk_origin, WORLD_CHUNK_SURROUNDINGS[i], aux);
                world->chunks[i] = chunk_init(renderer, aux, &world->noise_state);
        }

        set_border_chunks(world);
}

void
world_destroy(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_destroy(world->chunks[i]);
        free(world->chunks);
}

// NOTE(fonsi): Lo que mas cuesta son los malloc y los free de los chunk_{init, destroy}
// por lo que lo mejor que puedo hacer es agruparlos en un solo malloc y un solo free
// edit: son como max 10 mallocs y 10 frees (con CHUNK_RENDER_DISTANCE 2), por lo que lo peor seria el mesh_init
void
world_update(struct World* world)
{
        // comprobar si player esta en el chunk origin
        if (!player_in_chunk_origin(world))
        {
                // actualizar el chunk origin al nuevo chunk
                vec3 new_chunk_origin;
                get_chunkpos_from_position(world->renderer->current_camera->position, new_chunk_origin);
                glm_vec3_copy(new_chunk_origin, world->chunk_origin);

                // liberar chunks que ya no esten en el radio de chunk origin
                for (uint k = 0; k < WORLD_CHUNK_COUNT; k++)
                {
                        bool keep_loaded = false;
                        for (uint i = 0; i < WORLD_CHUNK_SURROUNDINGS_COUNT; i++)
                        {
                                vec3 aux;
                                glm_vec3_add(world->chunk_origin, WORLD_CHUNK_SURROUNDINGS[i], aux);
                                if (glm_vec3_eqv(aux, world->chunks[k]->world_position))
                                        keep_loaded = true;
                        }

                        // TODO(fonsi): aqui ocurre el bug raro, no destruyo bien el chunk, algo pasa
                        if (!keep_loaded)
                        {
                                chunk_destroy(world->chunks[k]);
                                world->chunks[k] = NULL;
                        }
                }

                // inicializar los espacios vacios a nuevos chunks
                for (uint i = 0; i < WORLD_CHUNK_SURROUNDINGS_COUNT; i++)
                {
                        vec3 aux;
                        glm_vec3_add(world->chunk_origin, WORLD_CHUNK_SURROUNDINGS[i], aux);

                        bool exist = false;
                        for (uint k = 0; k < WORLD_CHUNK_COUNT; k++)
                                if (world->chunks[k] && glm_vec3_eqv(aux, world->chunks[k]->world_position))
                                        exist = true;

                        if (!exist)
                        {
                                bool set = false;
                                for (uint k = 0; k < WORLD_CHUNK_COUNT; k++)
                                        if (!world->chunks[k] && !set)
                                        {
                                                world->chunks[k] =
                                                    chunk_init(world->renderer, aux, &world->noise_state);
                                                set = true;
                                        }
                        }
                }

                // setear los nuevos chunks en el borde y si hay que prepararlos de nuevo
                for (uint k = 0; k < WORLD_CHUNK_COUNT; k++)
                {
                        vec3 aux;
                        glm_vec3_sub(world->chunks[k]->world_position, world->chunk_origin, aux);

                        bool was_border = world->chunks[k]->border;
                        if (glm_vec3_max(aux) == WORLD_CHUNK_RENDER_DISTANCE ||
                            glm_vec3_min(aux) == -WORLD_CHUNK_RENDER_DISTANCE)
                        {
                                world->chunks[k]->border   = true;
                                world->chunks[k]->prepared = false;
                        }
                        else if (was_border)
                        {
                                world->chunks[k]->border   = false;
                                world->chunks[k]->prepared = false;
                        }
                        else
                                world->chunks[k]->border = false;
                }
        }
}

void
world_render(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                if (!world->chunks[i]->prepared)
                {
                        chunk_prepare_render(world->chunks[i]);
                        world->chunks[i]->prepared = true;
                }

        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                if (world->chunks[i]->prepared)
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
                if (world->chunks[i] && glm_vec3_eqv(world->chunks[i]->world_position, chunk_world_position))
                        return world->chunks[i];
        return NULL;
}

bool
world_is_block_at(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position)
{
        struct value_index aux = get_block_index_value_coord(chunk_block_position);
        struct Chunk*      chunk;
        vec3               new_chunk_position;
        vec3               new_block_position;

        glm_vec3_copy(chunk_world_position, new_chunk_position);
        glm_vec3_copy(chunk_block_position, new_block_position);

        if (aux.index == 1)
                return (false);

        // modificar valor de chunk_world_position si el bloque esta fuera de chunk
        if (aux.value == -1)
        {
                new_chunk_position[aux.index]--;
                new_block_position[aux.index] = 15;
        }
        else if (aux.value == 16)
        {
                new_chunk_position[aux.index]++;
                new_block_position[aux.index] = 0;
        }

        // conseguir el chunk del bloque
        chunk = world_get_chunk(world, new_chunk_position);

        if(!chunk || !chunk->blocks[BLOCKOFFSET(new_block_position)].active)
                return(false);

        return(true);
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
player_in_chunk_origin(struct World* world)
{
        vec3 aux = GLM_VEC3_ZERO_INIT;
        get_chunkpos_from_position(world->renderer->current_camera->position, aux);
        return glm_vec3_eqv(world->chunk_origin, aux);
}

internal void
set_border_chunks(struct World* world)
{
        FOR_EACH_POSITION(i, j)
        {
                vec3 aux;
                glm_vec3_add(world->chunk_origin, (vec3){i, 0, j}, aux);
                struct Chunk* chunk = world_get_chunk(world, aux);

                chunk->border = (i == WORLD_CHUNK_RENDER_DISTANCE || i == -WORLD_CHUNK_RENDER_DISTANCE ||
                                 j == WORLD_CHUNK_RENDER_DISTANCE || j == -WORLD_CHUNK_RENDER_DISTANCE)
                                    ? true
                                    : false;
        }
}
