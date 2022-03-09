#ifndef WORLD_H_
#define WORLD_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

extern vec3 WORLD_CHUNK_SURROUNDINGS[];

#define WORLD_CHUNK_RENDER_DISTANCE 3
#define WORLD_CHUNK_SURROUNDINGS_COUNT ((WORLD_CHUNK_RENDER_DISTANCE * 2 + 1) * (WORLD_CHUNK_RENDER_DISTANCE * 2 + 1))
vec3 WORLD_CHUNK_SURROUNDINGS[WORLD_CHUNK_SURROUNDINGS_COUNT];

#define WORLD_CHUNK_SIDE_COUNT (WORLD_CHUNK_RENDER_DISTANCE * 2 + 1)
#define WORLD_CHUNK_COUNT WORLD_CHUNK_SURROUNDINGS_COUNT

#define BLOCKOFFSET(vec) ((uint) vec[0] + CHUNK_SIZE_X * (uint) vec[1] + CHUNK_SIZE_X * CHUNK_SIZE_Y * (uint) vec[2])
#define CHUNKOFFSET(vec) ((uint) vec[0] + WORLD_CHUNK_SIDE * (uint) vec[2])
#define FOR_EACH_POSITION(i, j)                                                              \
        for (int i = -WORLD_CHUNK_RENDER_DISTANCE; i < WORLD_CHUNK_RENDER_DISTANCE + 1; i++) \
                for (int j = -WORLD_CHUNK_RENDER_DISTANCE; j < WORLD_CHUNK_RENDER_DISTANCE + 1; j++)

struct World
{
        struct Renderer* renderer;
        struct Chunk**   chunks;
        fnl_state        noise_state;

        vec3 chunk_origin; // el chunk donde se encuentra el personaje/camara
};

extern void world_init(struct World* world, struct Renderer* renderer);
extern void world_destroy(struct World* world);
extern void world_update(struct World* world);
extern void world_render(struct World* world);

extern uint          world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position);
extern struct Chunk* world_get_chunk(struct World* world, vec3 chunk_world_position);
extern bool          world_block_exists(struct World* world, struct Chunk* chunk, vec3 chunk_block_position);

extern void          world_set_neighbor_chunks(struct Chunk* chunk, struct World* world);

#endif // WORLD_H_
