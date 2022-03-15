#ifndef WORLD_H_
#define WORLD_H_

#include "../gfx/window.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

extern vec3 WORLD_CHUNK_SURROUNDINGS[];

struct World
{
        uint           chunks_size;
        struct Chunk** chunks;
        vec3           chunks_origin, chunks_offset;

        // Cosa de jdah para ver si soluciona los problemas
        struct
        {
                struct
                {
                        size_t count, max;
                } load, mesh;
        } throttles;
};

extern void          world_init(struct World* world, struct Renderer* renderer);
extern void          world_destroy(struct World* world);
extern void          world_update(struct World* world);
extern void          world_render(struct World* world);
extern void          world_tick(struct World* world);
extern uint          world_get_block(struct World* world, vec3 position);
extern struct Chunk* world_get_chunk(struct World* world, vec3 position);
extern bool          world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position);
extern void          world_set_neighbor_chunks(struct Chunk* chunk, struct World* world);
extern void          world_set_center(struct World* world, vec3 center);
extern void          world_pos_to_block(vec3 position, vec3 dest);

#endif // WORLD_H_
