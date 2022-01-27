#ifndef WORLD_H_
#define WORLD_H_

#include "../utils/utils.h"
#include "../utils/types.h"
#include "chunk.h"

#define WORLD_CHUNK_COUNT 512

struct World
{
        // TODO(fonsi): implementar heightmap
        // struct Heightmap heigthmap;

        struct Camera* camera;
        struct Chunk chunks[512];
};

extern void world_init(struct World* world, struct Camera* camera);
extern void world_destroy(struct World* world);
extern void world_update(struct World* world);
extern void world_render(struct World* world);

#endif // WORLD_H_
