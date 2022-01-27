#ifndef CHUNK_H_
#define CHUNK_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "block.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 256
#define CHUNK_SIZE_Z 16
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

struct Chunk
{
        // TODO(fonsi): mesh implementation
        // struct Mesh mesh;

        // TODO(fonsi): heightmap implementation
        // struct Heightmap heightmap;

        struct Block blocks[CHUNK_BLOCK_COUNT];
        vec3 world_offset;
};

extern void chunk_init(struct Chunk* chunk, vec3 world_offset);
extern void chunk_update(struct Chunk* chunk);
extern void chunk_render(struct Chunk* chunk);

#endif // CHUNK_H_
