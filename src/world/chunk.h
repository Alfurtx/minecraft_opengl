#ifndef CHUNK_H_
#define CHUNK_H_

#include "../gfx/mesh.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "../utils/worldgen.h"
#include "block.h"

struct Chunk
{
};

extern void chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position);
extern void chunk_update(struct Chunk* chunk);
extern void chunk_destroy(struct Chunk* chunk);
extern void chunk_generate_mesh(struct Chunk* chunk);
extern void chunk_render(struct Chunk* chunk);
extern uint chunk_get_block(struct Chunk* chunk, vec3 position);
extern void chunk_create_map(struct Chunk* chunk);

#endif // CHUNK_H_
