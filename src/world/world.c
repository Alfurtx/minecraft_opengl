#include "world.h"

struct value_index
{
        float value;
        int   index;
};

// NOTE(fonsi): si block_position no tiene ninguna coordenada fuera del rango de un chunk (ej. -1, 16 o 256), devuelve
// index = -1
internal inline struct value_index get_block_index_value_coord(vec3 block_position);

internal inline void get_chunkpos_from_position(vec3 position, vec3 dest);
internal inline bool player_in_chunk_origin(struct World* world);
internal inline void set_border_chunks(struct World* world);

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
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
        {
                vec3 aux;
                glm_vec3_add(world->chunk_origin, WORLD_CHUNK_SURROUNDINGS[i], aux);
                world->chunks[i] = chunk_init(renderer, aux, &world->noise_state);
        }

        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                world_set_neighbor_chunks(world->chunks[i], world);

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

                        if(was_border != world->chunks[k]->border)
                                world_set_neighbor_chunks(world->chunks[k], world);
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

uint
world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position)
{
        struct value_index aux = get_block_index_value_coord(chunk_block_position);
        struct Chunk*      chunk;
        vec3               new_chunk_position;
        vec3               new_block_position;

        glm_vec3_copy(chunk_world_position, new_chunk_position);
        glm_vec3_copy(chunk_block_position, new_block_position);

        if (aux.index == 1)
                return 0;

        // modificar valor de chunk_world_position si el bloque esta fuera de chunk
        if (aux.index != -1)
        {
                if (aux.value == -1)
                {
                        new_chunk_position[aux.index]--;
                        new_block_position[aux.index] = CHUNK_SIZE_X - 1;
                }
                if (aux.value == CHUNK_SIZE_X)
                {
                        new_chunk_position[aux.index]++;
                        new_block_position[aux.index] = 0;
                }
        }

        // conseguir el chunk del bloque
        chunk = world_get_chunk(world, new_chunk_position);

        if (!chunk)
                return 0;
        else
                return chunk->blocks[BLOCKOFFSET(new_block_position)];
}

struct Chunk*
world_get_chunk(struct World* world, vec3 chunk_world_position)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
        {
                struct Chunk* aux = world->chunks[i];

                if (aux && world->chunks[i]->world_position[0] && chunk_world_position[0] && aux->world_position[1] &&
                    chunk_world_position[1] && aux->world_position[2] && chunk_world_position[2])
                        return aux;
        }
        return NULL;
}

bool
world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position)
{
        struct value_index pair = get_block_index_value_coord(chunk_block_position);
        if(pair.index == -1)
        {
                return chunk->blocks[BLOCKOFFSET(chunk_block_position)] & BLOCK_MASK_ACTIVE;
        }

        // vec3 new_block_pos;
        // glm_vec3_copy(chunk_block_position, new_block_pos);
        // if (pair.value == -1)
        // {
        //         if(pair.index == 0 && chunk->xn)
        //         {
        //                 new_block_pos[pair.index] = CHUNK_SIZE_X - 1;
        //                 return chunk->xn->blocks[BLOCKOFFSET(new_block_pos)] & BLOCK_MASK_ACTIVE;
        //         }
        //         if(pair.index == 2 && chunk->zn)
        //         {
        //                 new_block_pos[pair.index] = CHUNK_SIZE_Z - 1;
        //                 return chunk->zn->blocks[BLOCKOFFSET(new_block_pos)] & BLOCK_MASK_ACTIVE;
        //         }
        // }
        // if (pair.value == CHUNK_SIZE_X)
        // {
        //         if(pair.index == 0 && chunk->xp)
        //         {
        //                 new_block_pos[pair.index] = 0;
        //                 return chunk->xp->blocks[BLOCKOFFSET(new_block_pos)] & BLOCK_MASK_ACTIVE;
        //         }
        //         if(pair.index == 2 && chunk->zp)
        //         {
        //                 new_block_pos[pair.index] = 0;
        //                 return chunk->zp->blocks[BLOCKOFFSET(new_block_pos)] & BLOCK_MASK_ACTIVE;
        //         }
        // }

        return(false);
}

void
world_set_neighbor_chunks(struct Chunk* chunk, struct World* world)
{
        for(uint i = 0; i < WORLD_CHUNK_COUNT; i++)
        {
                struct Chunk* aux = world->chunks[i];
                if(aux)
                {
                        if(glm_vec3_eqv(aux->world_position, (vec3){chunk->world_position[0] + 1, chunk->world_position[1], chunk->world_position[2]}))
                                chunk->xp = aux;
                        else chunk->xp = NULL;

                        if(glm_vec3_eqv(aux->world_position, (vec3){chunk->world_position[0] - 1, chunk->world_position[1], chunk->world_position[2]}))
                                chunk->xn = aux;
                        else chunk->xn = NULL;

                        if(glm_vec3_eqv(aux->world_position, (vec3){chunk->world_position[0], chunk->world_position[1], chunk->world_position[2] + 1}))
                                chunk->zp = aux;
                        else chunk->zp = NULL;

                        if(glm_vec3_eqv(aux->world_position, (vec3){chunk->world_position[0], chunk->world_position[1], chunk->world_position[2] - 1}))
                                chunk->zn = aux;
                        else chunk->zn = NULL;
                }
        }
}

internal inline void
get_chunkpos_from_position(vec3 position, vec3 dest)
{
        dest[0] = floorf(position[0] / CHUNK_SIZE_X);
        dest[1] = 0;
        dest[2] = floorf(position[2] / CHUNK_SIZE_Z);
}

internal inline struct value_index
get_block_index_value_coord(vec3 block_position)
{
        struct value_index res = {0, -1};
        for (uint i = 0; i < 3; i++)
        {
                if (block_position[i] == -1 || block_position[i] == CHUNK_SIZE_X || block_position[i] == CHUNK_SIZE_Y)
                {
                        res.index = i;
                        res.value = block_position[i];
                        return res;
                }
        }
        return res;
}

internal inline bool
player_in_chunk_origin(struct World* world)
{
        vec3 aux = GLM_VEC3_ZERO_INIT;
        get_chunkpos_from_position(world->renderer->current_camera->position, aux);
        return glm_vec3_eqv(world->chunk_origin, aux);
}

internal inline void
set_border_chunks(struct World* world)
{
        FOR_EACH_POSITION(i, j)
        {
                vec3 aux;
                glm_vec3_add(world->chunk_origin, (vec3){i, 0, j}, aux);
                struct Chunk* chunk = world_get_chunk(world, aux);

                if (chunk)
                {
                        chunk->border = (i == WORLD_CHUNK_RENDER_DISTANCE || i == -WORLD_CHUNK_RENDER_DISTANCE ||
                                         j == WORLD_CHUNK_RENDER_DISTANCE || j == -WORLD_CHUNK_RENDER_DISTANCE)
                                            ? true
                                            : false;
                }
        }
}
