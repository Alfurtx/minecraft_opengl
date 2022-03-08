#ifndef CHUNK_H_
#define CHUNK_H_

#include "../gfx/mesh.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "../utils/worldgen.h"
#include "block.h"

#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32
#define CHUNK_BLOCK_COUNT (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

#define BLOCK_MASK_ACTIVE 0xffffff00
#define BLOCK_MASK_TYPE 0xffff00ff

struct Chunk
{
        struct Mesh      mesh;
        struct Renderer* renderer;

        /*
        ** BYTE FIELDS
        ** -----------
        ** byte 0 -> active
        ** byte 1 -> type
        */
        uint blocks[CHUNK_BLOCK_COUNT];

        struct Heightmap heightmap;

        vec3 world_offset;
        vec3 world_position;

        bool border;
        bool prepared;
};

extern struct Chunk* chunk_init(struct Renderer* renderer, vec3 world_position, fnl_state* noise_state);
extern void          chunk_update(struct Chunk* chunk);
extern void          chunk_destroy(struct Chunk* chunk);
extern void          chunk_prepare_render(struct Chunk* chunk);
extern void          chunk_render(struct Chunk* chunk);

#endif // CHUNK_H_
