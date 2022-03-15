#include "world.h"

#define WORLD_GETINDEX(pos) (WORLD_CHUNKSIZE * pos[2] + pos[0])
#define WORLD_FOREACH(_world) for (uint i = 0; i < ((_world)->chunks_size * (_world)->chunks_size); i++)

internal inline void world_block_to_chunk(vec3 position, vec3 dest);
internal void        world_load_chunk(struct World* world, vec3 offset);
internal uint        world_chunk_index(struct World* world, vec3 offset);
internal bool        world_contains_chunk(struct World* world, vec3 pos);
internal void        world_load_empty_chunks(struct World* world);
internal void        world_chunk_offset(struct World* world, uint i, vec3 dest);

void
world_init(struct World* world, struct Renderer* renderer)
{
        memset(world, 0, sizeof(struct World));
        world->throttles.load.max = 2;
        world->throttles.mesh.max = 2;
        world->chunks_size        = 10;
        world->chunks             = calloc(1, world->chunks_size * world->chunks_size * sizeof(struct Chunk*));
}

void
world_destroy(struct World* world)
{
        WORLD_FOREACH(world)
        {
                struct Chunk* chunk = world->chunks[i];
                if (chunk)
                {
                        chunk_destroy(chunk);
                        free(chunk);
                }
        }

        free(world->chunks);
}

void
world_update(struct World* world)
{
        world->throttles.mesh.count = 0;
        world->throttles.load.count = 0;
        world_load_empty_chunks(world);
}

void
world_render(struct World* world)
{
        WORLD_FOREACH(world)
        {
                struct Chunk* chunk = world->chunks[i];
                if (chunk)
                        chunk_render(chunk);
        }
}

void
world_tick(struct World* world)
{
}

uint
world_get_block(struct World* world, vec3 position)
{
        vec3 chunk_pos;
        world_block_to_chunk(position, chunk_pos);
        struct Chunk* aux = world_get_chunk(world, chunk_pos);

        if (position[1] <= 0 && position[1] >= CHUNK_SIZE_Y - 1 && (aux != NULL))
                return chunk_get_block(aux, position);

        return 0;
}

struct Chunk*
world_get_chunk(struct World* world, vec3 offset)
{
        vec3 aux;
        glm_vec3_sub(offset, world->chunks_origin, aux);
        if (aux[0] >= 0 && aux[2] >= 0 && aux[0] <= world->chunks_size && aux[2] <= world->chunks_size)
                return world->chunks[world_chunk_index(world, aux)];
        else
                return NULL;
}

bool
world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position)
{
        return false;
}

void
world_set_neighbor_chunks(struct Chunk* chunk, struct World* world)
{
}

internal inline void
world_block_to_chunk(vec3 position, vec3 dest)
{
        dest[0] = floorf(position[0] / CHUNK_SIZE_X);
        dest[1] = 0;
        dest[2] = floorf(position[2] / CHUNK_SIZE_Z);
}

internal void
world_load_chunk(struct World* world, vec3 offset)
{
        struct Chunk* chunk = malloc(sizeof(struct Chunk));
        chunk_init(chunk, world, offset);
        chunk_create_map(chunk);
        world->chunks[world_chunk_index(world, offset)] = chunk;
}

internal uint
world_chunk_index(struct World* world, vec3 offset)
{
        vec3 aux;
        glm_vec3_sub(offset, world->chunks_origin, aux);
        return aux[2] * world->chunks_size + aux[0];
}

void
world_set_center(struct World* world, vec3 center)
{
        vec3 new_offset, new_origin;
        world_block_to_chunk(center, new_offset);
        glm_vec3_sub(new_offset, (vec3){(world->chunks_size / 2) - 1, 0, (world->chunks_size / 2) - 1}, new_origin);

        if (!memcmp(&new_origin, world->chunks_origin, sizeof(vec3)))
                return;

        glm_vec3_copy(new_offset, world->chunks_offset);
        glm_vec3_copy(new_origin, world->chunks_origin);

        uint          size = world->chunks_size * world->chunks_size;
        struct Chunk* old[size];
        memcpy(old, world->chunks, size * sizeof(struct Chunk*));
        memset(world->chunks, 0, size * sizeof(struct Chunk*));

        for (uint i = 0; i < size; i++)
        {
                struct Chunk* chunk = old[i];
                if (!chunk)
                        continue;
                else if (world_contains_chunk(world, chunk->offset))
                        world->chunks[world_chunk_index(world, chunk->offset)] = chunk;
                else
                {
                        chunk_destroy(chunk);
                        free(chunk);
                }
        }

        world_load_empty_chunks(world);
}

void
world_pos_to_block(vec3 position, vec3 dest)
{
        dest[0] = floorf(position[0]);
        dest[1] = floorf(position[1]);
        dest[2] = floorf(position[2]);
}

internal bool
world_contains_chunk(struct World* world, vec3 pos)
{
        vec3 aux;
        glm_vec3_sub(pos, world->chunks_origin, aux);
        return aux[0] >= 0 && aux[2] && aux[0] < world->chunks_size && aux[2] < world->chunks_size;
}

internal void
world_load_empty_chunks(struct World* world)
{
        for (uint i = 0; i < world->chunks_size * world->chunks_size; i++)
        {
                if (!world->chunks[i] && world->throttles.load.count < world->throttles.load.max)
                {
                        vec3 aux;
                        world_chunk_offset(world, i, aux);
                        world_load_chunk(world, aux);
                        world->throttles.load.count++;
                }
        }
}

internal void
world_chunk_offset(struct World* world, uint i, vec3 dest)
{
        glm_vec3_add(world->chunks_origin, (vec3){i % world->chunks_size, 0, 1 / world->chunks_size}, dest);
}
