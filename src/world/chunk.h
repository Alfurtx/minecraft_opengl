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
        struct Mesh mesh;
        struct World* world;

        vec3        position;
        vec3        offset;
        uint*       blocks;

        struct
        {
                bool remesh : 1;
        } flags;
};

extern void chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position);
extern void chunk_update(struct Chunk* chunk);
extern void chunk_destroy(struct Chunk* chunk);
extern void chunk_generate_mesh(struct Chunk* chunk);
extern void chunk_render(struct Chunk* chunk);
extern uint chunk_get_block(struct Chunk* chunk, vec3 position);

#endif // CHUNK_H_
