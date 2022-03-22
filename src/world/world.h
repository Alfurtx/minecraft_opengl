#ifndef WORLD_H_
#define WORLD_H_

#include "../gfx/window.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

struct World
{
        uint             chunks_size;
        struct Chunk**    chunks;
        struct Renderer* renderer;

        vec3 chunks_origin, chunks_offset;

        struct
        {
                uint count;
                uint max;
        } mesh_queue, load_queue;
};

extern void world_init(struct World* world, struct Renderer* renderer);
extern void world_destroy(struct World* world);
extern void world_update(struct World* world);
extern void world_render(struct World* world);
extern void world_tick(struct World* world);

// NOTE(fonsi): el offset es el chunk con su posicion ya escalada a CHUNK_SIZE
extern struct Chunk* world_get_chunk(struct World* world, vec3 offset);
extern uint          world_get_block(struct World* world, vec3 offset);

#endif // WORLD_H_
