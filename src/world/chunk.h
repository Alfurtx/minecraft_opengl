#ifndef CHUNK_H_
#define CHUNK_H_

#include "../gfx/mesh.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "block.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 256
#define CHUNK_SIZE_Z 16
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

struct Chunk
{
        struct Mesh      mesh;
        struct Renderer* renderer;
        struct Block     blocks[CHUNK_BLOCK_COUNT];

        vec3 world_offset;
        vec3 world_position;

        bool border;
        bool prepared;
};

extern struct Chunk* chunk_init(struct Renderer* renderer, vec3 world_position);
extern void          chunk_update(struct Chunk* chunk);
extern void          chunk_destroy(struct Chunk* chunk);
extern void          chunk_prepare_render(struct Chunk* chunk);
extern void          chunk_render(struct Chunk* chunk);

#endif // CHUNK_H_
