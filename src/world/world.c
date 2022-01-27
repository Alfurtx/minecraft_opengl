#include "world.h"

void
world_init(struct World* world, struct Camera* camera)
{
        block_init();

        // TODO(fonsi): hacer sistema de mundo infinito y no tan estatico
        for(uint i = 0; i < 16; i++)
                for(uint j = 0; j < 16; j++)
                        chunk_init(&world->chunks[i * 16 + j], (vec3){j * 16, 0, i * 16});
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
        for(uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_render(&world->chunks[i]);
}
