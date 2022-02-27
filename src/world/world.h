#ifndef WORLD_H_
#define WORLD_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"


extern vec3 WORLD_CHUNK_SURROUNDINGS[];

struct World
{
        struct Renderer* renderer;
        struct Chunk**   chunks;
        fnl_state noise_state;

        vec3 chunk_origin; // el chunk donde se encuentra el personaje/camara
};

extern void world_init(struct World* world, struct Renderer* renderer);
extern void world_destroy(struct World* world);
extern void world_update(struct World* world);
extern void world_render(struct World* world);

extern struct Block  world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position);
extern struct Chunk* world_get_chunk(struct World* world, vec3 chunk_world_position);

#endif // WORLD_H_
