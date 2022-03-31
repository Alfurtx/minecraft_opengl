#include "world.h"

// IMPORTANT(fonsi): Recordar que las coordenadas con las que trabaja world son siempre globales, es decir, tienen valores negativos en sus coordenadas

#define foreach_chunk(_w) for (uint i = 0; i < (_w)->chunks_count; i++)
#define block_index(pos) ((uint) (pos[0] + pos[1] * CHUNK_SIZE + pos[2] * CHUNK_SIZE * CHUNK_SIZE))
#define chunk_index(w, pos)                                            \
        ((uint) ((pos[2] - (w)->chunks_origin[2]) * (w)->chunks_size + \
                 (pos[0] - (w)->chunks_origin[0])))

internal inline bool chunk_exists(struct World* world, vec3 offset);
internal inline void load_chunk(struct World* world, vec3 offset);
internal void        load_empty_chunks(struct World* world);

// world position -> block coordinates in some chunk
internal inline void position_to_block(vec3 pos, vec3 dest);

// world position -> chunk position
internal inline void position_to_chunk(vec3 pos, vec3 dest);

// chunk offset -> chunk array index
internal inline uint offset_to_index(struct World* world, vec3 offset);

// chunk array index -> chunk offset
internal inline void index_to_offset(struct World* world, uint i, vec3 dest);

// world position -> block array index in chunk
internal inline uint block_index(struct World* world, vec3 block_world_position);

void
world_init(struct World* world, struct Renderer* renderer)
{
        memset(world, 0, sizeof(struct World));

        world->chunks_size  = 10;
        world->chunks_count = world->chunks_size * world->chunks_size;
        world->chunks       = calloc(world->chunks_count, sizeof(struct Chunk*));
        glm_vec3_zero(world->chunks_offset);
        glm_vec3_sub(world->chunks_offset,
                     (vec3){(float) world->chunks_size / 2, 0, (float) world->chunks_size / 2},
                     world->chunks_origin);

        world->renderer = renderer;

        world->mesh_queue.count = 0;
        world->mesh_queue.max   = 2;
        world->load_queue.count = 0;
        world->load_queue.max   = 2;

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
        position_to_chunk(center, new_offset);

        if (glm_vec3_eq(world->chunk_offset, new_offset))
                return;

        glm_vec3_sub(new_offset,
                     (vec3){(float) world->chunks_size / 2, 0, (float) world->chunks_size / 2},
                     world->chunks_origin);

        struct Chunk* old[world->chunks_count];
        memcpy(old, world->chunks, sizeof(struct Chunk*) * world->chunks_count);
        memset(world->chunks, 0, sizeof(struct Chunk*) * world->chunks_count);

        foreach_chunk(world)
        {
                struct Chunk* c = old[i];
                if (c == NULL)
                        continue;
                else if (chunk_exists(world, c->offset))
                        world->chunks[chunk_index(world, c->offset)] = c;
                else {
                        chunk_destroy(c);
                        free(c);
                }
        }

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
        return chunk_exists(world, offset) ? world->chunks[chunk_index(world, offset)] : NULL;
}

uint
world_get_block(struct World* world, vec3 offset)
{
        vec3 coff;
        vec3 boff;

        world_position_to_block(offset, boff);
        position_to_chunk(offset, coff);
        struct Chunk* c = world_get_chunk(world, coff);

        return c != NULL ? c->blocks[block_index(boff)] : 0;
}

internal void
load_empty_chunks(struct World* world)
{
        foreach_chunk(world)
        {
                struct Chunk* c = world->chunks[i];
                if (!c && world->load_queue.count < world->load_queue.max) {
                        vec3 coffset;
                        index_to_offset(world, i, coffset);
                        load_chunk(world, coffset);
                        world->load_queue.count++;
                }
        }
}

internal inline void
position_to_block(vec3 pos, vec3 dest)
{
        if(pos[1] < 0 || pos[1] >= CHUNK_SIZE) pos[1] = CHUNK_SIZE - 1;
        vec3 chunk_position, chunk_offset, block_chunk_position;
        position_to_chunk(pos, chunk_position);
        glm_vec3_scale(chunk_position, CHUNK_SIZE, chunk_offset);
        glm_vec3_sub(pos, chunk_offset, dest);
}

internal inline void
position_to_chunk(vec3 pos, vec3 dest)
{
        dest[0] = floorf(pos[0] / CHUNK_SIZE);
        dest[1] = 0;
        dest[2] = floorf(pos[2] / CHUNK_SIZE);
}

internal inline uint
offset_to_index(struct World* world, vec3 offset)
{
        return ((offset[0] - CHUNK_SIZE) * world->chunks_size + (offset[2] - CHUNK_SIZE));
}

internal inline void
index_to_offset(struct World* world, uint i, vec3 dest)
{
        glm_vec3_add(world->chunks_origin,
                     (vec3){i % world->chunks_size, 0, (float) i / world->chunks_size}, dest);
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
        struct Chunk* chunk = malloc(sizeof(*chunk));
        chunk_init(chunk, world, offset);
        chunk_create_map(chunk);
        world->chunks[chunk_index(world, offset)] = chunk;
}

internal inline uint
block_index(struct World* world, vec3 block_world_position)
{
        vec3 result;
        position_to_block(block_world_position, result);
        return block_index(result);
}
