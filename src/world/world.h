#ifndef WORLD_H_
#define WORLD_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

#define WORLD_CHUNK_COUNT 16
#define WORLD_STATIC_SIDE 4

struct World
{
        // TODO(fonsi): implementar heightmap
        // struct Heightmap heigthmap;

        struct Renderer* renderer;
        struct Chunk**     chunks;

        vec3 chunk_origin; // el chunk donde se encuentra el personaje/camara
};

extern void world_init(struct World* world, struct Renderer* renderer);
extern void world_destroy(struct World* world);
extern void world_update(struct World* world);
extern void world_render(struct World* world);

extern struct Block world_get_block(struct World* world, vec3 chunk_world_position, vec3 chunk_block_position);
extern struct Chunk* world_get_chunk(struct World* world, vec3 chunk_world_position);

#endif // WORLD_H_
