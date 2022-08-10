#include "world.h"

// IMPORTANT(fonsi): Recordar que las coordenadas con las que trabaja world son siempre globales, es
// decir, tienen valores negativos en sus coordenadas

#define DEFAULT_WORLD_CHUNK_SIZE 20

#define foreach_chunk(_w) for (uint i = 0; i < (_w)->chunks_count; i++)
#define block_index(pos) ((uint) (pos[0] + pos[1] * CHUNK_SIZE + pos[2] * CHUNK_SIZE * CHUNK_SIZE))
#define chunk_index(w, pos)                                            \
        ((uint) ((pos[2] - (w)->chunks_origin[2]) * (w)->chunks_size + \
                 (pos[0] - (w)->chunks_origin[0])))

internal inline bool chunk_exists(struct World* world, vec3 offset);
internal inline void load_chunk(struct World* world, vec3 offset);
internal void        load_empty_chunks(struct World* world);

internal inline uint offset_to_index(struct World* world, vec3 offset);
internal inline uint position_to_index(struct World* world, vec3 position);
internal inline void position_to_block(vec3 pos, vec3 dest);
internal inline void position_to_offset(vec3 pos, vec3 dest);
internal inline void position_to_chunk_position(vec3 pos, vec3 dest);
internal inline void index_to_position(struct World* world, uint i, vec3 dest);

void
world_init(struct World* world, struct Renderer* renderer)
{
        memset(world, 0, sizeof(struct World));

        world->chunks_size  = 20;
        world->chunks_count = world->chunks_size * world->chunks_size;
        world->chunks       = calloc(world->chunks_count, sizeof(struct Chunk*));
        glm_vec3_zero(world->chunks_offset);
        glm_vec3_sub(world->chunks_offset,
                     (vec3){(float) world->chunks_size / 2, 0, (float) world->chunks_size / 2},
                     world->chunks_origin);

        world->renderer = renderer;

        world->mesh_queue.max = 2;
        world->load_queue.max = 2;

        world_set_center(world, GLM_VEC3_ZERO);
}

void
world_destroy(struct World* world)
{
        foreach_chunk(world)
        {
                struct Chunk* c = world->chunks[i];
                if (c)
                        chunk_destroy(c);
        }

        free(world->chunks);
}

void
world_update(struct World* world)
{
        world->load_queue.count = 0;
        world->mesh_queue.count = 0;
        load_empty_chunks(world);
}

void
world_render(struct World* world)
{
        foreach_chunk(world)
        {
                struct Chunk* c = world->chunks[i];
                if (c)
                        chunk_render(c);
        }
}

void
world_tick(struct World* world)
{
}

void
world_set_center(struct World* world, vec3 center)
{
        vec3 new_offset;
        position_to_chunk_position(center, new_offset);

        if (glm_vec3_eqv(world->chunks_offset, new_offset))
                return;

        glm_vec3_sub(new_offset, (vec3){world->chunks_size / 2, 0, world->chunks_size / 2},
                     world->chunks_origin);

        // IMPORTANT(fonsi): changed to DEFAULT_WORLD_CHUNK_SIZE instead of world->chunks_count because CL was complaining on Windows
        struct Chunk** old = _malloca(sizeof(struct Chunk*) * world->chunks_count);
        memcpy(old, world->chunks, sizeof(struct Chunk*) * world->chunks_count);
        memset(world->chunks, 0, sizeof(struct Chunk*) * world->chunks_count);

        foreach_chunk(world)
        {
                struct Chunk* c = old[i];
                if (c == NULL)
                        continue;
                else if (chunk_exists(world, c->position))
                        world->chunks[position_to_index(world, c->position)] = c;
                else {
                        chunk_destroy(c);
                        free(c);
                        c = NULL;
                }
        }

        _freea(old);
        load_empty_chunks(world);
}

void
world_position_to_block(vec3 pos, vec3 dest)
{
        position_to_block(pos, dest);
}

struct Chunk*
world_get_chunk(struct World* world, vec3 offset)
{
        return chunk_exists(world, offset) ? world->chunks[offset_to_index(world, offset)] : NULL;
}

uint
world_get_block(struct World* world, vec3 offset)
{
        vec3 coff;
        vec3 boff;

        world_position_to_block(offset, boff);
        position_to_offset(offset, coff);
        struct Chunk* c = world_get_chunk(world, coff);

        return c != NULL ? c->blocks[block_index(boff)] : 0;
}

internal void
load_empty_chunks(struct World* world)
{
        foreach_chunk(world)
        {
                struct Chunk* c = world->chunks[i];
                if (c == NULL && world->load_queue.count < world->load_queue.max) {
                        vec3 coffset;
                        index_to_position(world, i, coffset);
                        load_chunk(world, coffset);
                        world->load_queue.count++;
                }
        }
}

internal inline bool
chunk_exists(struct World* world, vec3 offset)
{
        vec3 pos;
        glm_vec3_sub(offset, world->chunks_origin, pos);
        return pos[0] >= 0 && pos[2] >= 0 && pos[0] < world->chunks_size &&
               pos[2] < world->chunks_size;
}

internal inline void
load_chunk(struct World* world, vec3 offset)
{
        struct Chunk* chunk = malloc(sizeof(struct Chunk));
        chunk_init(chunk, world, offset);
        worldgen_generate(chunk);
        world->chunks[chunk_index(world, offset)] = chunk;
}

internal inline uint
offset_to_index(struct World* world, vec3 offset)
{
        vec3 pos;
        pos[0] = (int) offset[0] / CHUNK_SIZE;
        pos[2] = (int) offset[2] / CHUNK_SIZE;
        return ((pos[2] - world->chunks_origin[2]) * world->chunks_size +
                (pos[0] - world->chunks_origin[0]));
}

internal inline uint
position_to_index(struct World* world, vec3 position)
{
        return ((position[2] - world->chunks_origin[2]) * world->chunks_size +
                (position[0] - world->chunks_origin[0]));
}

internal inline void
position_to_block(vec3 pos, vec3 dest)
{
        if (pos[1] < 0 || pos[1] > CHUNK_SIZE)
                dest[1] = CHUNK_SIZE - 1;

        vec3 offset;
        offset[0] = (int) pos[0] / CHUNK_SIZE;
        offset[1] = (int) pos[1] / CHUNK_SIZE;
        offset[2] = (int) pos[2] / CHUNK_SIZE;
        glm_vec3_scale(offset, CHUNK_SIZE, offset);
        glm_vec3_sub(pos, offset, dest);

        for (uint i = 0; i < 3; i++)
                dest[i] = fabsf(dest[i]);
}

internal inline void
position_to_offset(vec3 pos, vec3 dest)
{
        vec3 offset;
        offset[0] = (int) pos[0] / CHUNK_SIZE;
        offset[1] = 0;
        offset[2] = (int) pos[2] / CHUNK_SIZE;
        glm_vec3_scale(offset, CHUNK_SIZE, dest);
}

internal inline void
index_to_position(struct World* world, uint i, vec3 dest)
{
        glm_vec3_add(world->chunks_origin,
                     (vec3){i % world->chunks_size, 0, i / world->chunks_size}, dest);
}

internal inline void
position_to_chunk_position(vec3 pos, vec3 dest)
{
        dest[0] = (int) pos[0] / CHUNK_SIZE;
        dest[1] = 0;
        dest[2] = (int) pos[2] / CHUNK_SIZE;
}
