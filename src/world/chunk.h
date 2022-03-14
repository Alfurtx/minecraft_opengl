#ifndef CHUNK_H_
#define CHUNK_H_

#include "../gfx/mesh.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "../utils/worldgen.h"
#include "block.h"

#define CHUNK_SIZE 32
#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

#define BLOCK_MASK_ACTIVE 0x000000FF
#define BLOCK_MASK_TYPE 0x0000FF00

struct Chunk
{
        struct World*    world;
        struct Mesh      mesh;
        struct Renderer* renderer;

        /*
        ** BYTE FIELDS
        ** -----------
        ** byte 0 -> active
        ** byte 1 -> type
        */
        uint* blocks;

        struct
        {
                bool remesh : 1;
        } flags;
};

extern void chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position);
extern void chunk_update(struct Chunk* chunk);
extern void chunk_destroy(struct Chunk* chunk);
extern void chunk_prepare_render(struct Chunk* chunk);
extern void chunk_render(struct Chunk* chunk);

#endif // CHUNK_H_
