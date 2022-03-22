#include "world.h"

internal void world_set_chunks(struct World* world);
internal void get_offset(vec3 position, vec3 dest);

void
world_init(struct World* world, struct Renderer* renderer)
{
        memset(world, 0, sizeof(struct World));
        world->renderer         = renderer;
        world->chunks_size      = 10;
        world->chunks           = calloc(world->chunks_size * world->chunks_size, sizeof(struct Chunk*));
        world->mesh_queue.count = 0;
        world->load_queue.count = 0;
        world->mesh_queue.max   = 2;
        world->load_queue.max   = 2;
}

void
world_destroy(struct World* world)
{
        free(world->chunks);
}

void
world_update(struct World* world)
{
        world->mesh_queue.count = 0;
        world->load_queue.count = 0;
}

void
world_render(struct World* world)
{
}

void
world_tick(struct World* world)
{
}

internal void
world_set_chunks(struct World* world)
{
        vec3 origin, offset;
        get_offset(world->chunks_offset, offset);
        glm_vec3_sub(offset, (vec3){world->chunks_size / 2 - 1, 0, world->chunks_size / 2 - 1}, origin);

        if (!memcmp(origin, world->chunks_origin, sizeof(float) * 3))
                return;

        glm_vec3_copy(origin, world->chunks_origin);
        glm_vec3_copy(offset, world->chunks_offset);

        uint          aux = world->chunks_size * world->chunks_size;
        struct Chunk** old;
        memcpy(old, world->chunks, aux * sizeof(struct Chunk*));

        memset(world->chunks, 0, aux * sizeof(struct Chunk*));

        for (uint i = 0; i < aux; i++)
        {
                struct Chunk* c = old[i];
                if(!c)
                        continue;
                else if()
                else
        }
}

internal void
get_offset(vec3 position, vec3 dest)
{
        dest[0] = floorf(position[0] / CHUNK_SIZE);
        dest[1] = 0;
        dest[2] = floorf(position[2] / CHUNK_SIZE);
}
