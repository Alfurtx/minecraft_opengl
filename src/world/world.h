#ifndef WORLD_H_
#define WORLD_H_

#include "../gfx/window.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

// IMPORTANT(fonsi):
// POSITION -> WORLD POSITION
// OFFSET -> WORLD POSITION * CHUNK SIZE
//
// eg.
// position (-5, 0, -4)
// offset (-160, 0, -128)

struct World
{
        struct Chunk** chunks;
        uint           chunks_size;
        uint           chunks_count;

        struct Renderer* renderer;

        vec3 chunks_origin, chunks_offset;

        struct
        {
                uint count, max;
        } mesh_queue, load_queue;
};

extern void          world_init(struct World* world, struct Renderer* renderer);
extern void          world_destroy(struct World* world);
extern void          world_update(struct World* world);
extern void          world_render(struct World* world);
extern void          world_tick(struct World* world);
extern struct Chunk* world_get_chunk(struct World* world, vec3 offset);
extern uint          world_get_block(struct World* world, vec3 offset);
extern void          world_position_to_block(vec3 pos, vec3 dest);
extern void          world_set_center(struct World* world, vec3 center);

#endif // WORLD_H_
