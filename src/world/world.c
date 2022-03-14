#include "world.h"

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
world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position)
{
}

struct Chunk*
world_get_chunk(struct World* world, vec3 chunk_world_position)
{
}

bool
world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position)
{
}

void
world_set_neighbor_chunks(struct Chunk* chunk, struct World* world)
{
}

