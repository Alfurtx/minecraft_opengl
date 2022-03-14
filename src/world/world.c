#include "world.h"

#define WORLD_GETINDEX(pos) (WORLD_CHUNKSIZE * pos[2] + pos[0])

internal inline void world_block_to_chunk(vec3 position, vec3 dest);

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
        glm_vec3_sub(offset, world->chunk_origin, aux);
        if (aux[0] >= 0 && aux[2] >= 0 && aux[0] <= WORLD_CHUNKSIZE && aux[2] <= WORLD_CHUNKSIZE)
                return world->chunks[(uint) WORLD_GETINDEX(aux)];
        else
                return NULL;
}

bool
world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position)
{
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
