#ifndef WORLD_H_
#define WORLD_H_

#include "../gfx/window.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

extern vec3 WORLD_CHUNK_SURROUNDINGS[];

// El cuadrado de chunks que se van a mantener es de tamaño WORLD_CHUNKSIZE * WORLD_CHUNKSIZE
#define WORLD_CHUNKSIZE 10

struct World
{
        struct Renderer* renderer;
        struct Chunk** chunks;
};

extern void          world_init(struct World* world, struct Renderer* renderer);
extern void          world_destroy(struct World* world);
extern void          world_update(struct World* world);
extern void          world_render(struct World* world);
extern uint          world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position);
extern struct Chunk* world_get_chunk(struct World* world, vec3 chunk_world_position);
extern bool          world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position);
extern void          world_set_neighbor_chunks(struct Chunk* chunk, struct World* world);

#endif // WORLD_H_
