#ifndef CHUNK_H_
#define CHUNK_H_

#include "../gfx/mesh.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "../utils/worldgen.h"
#include "block.h"

#define CHUNK_SIZE 32
#define BLOCK_COUNT (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

struct Chunk {
        struct World* world;
        struct Mesh   mesh;
        uint*         blocks;
        vec3          position;
        vec3          offset;
        bool          remesh;
};

extern void chunk_init(struct Chunk* chunk, struct World* world, vec3 world_position);
extern void chunk_update(struct Chunk* chunk);
extern void chunk_destroy(struct Chunk* chunk);
extern void chunk_generate_mesh(struct Chunk* chunk);
extern void chunk_render(struct Chunk* chunk);
extern uint chunk_get_block(struct Chunk* chunk, vec3 position);
extern void chunk_set_block(struct Chunk* chunk, vec3 position, enum BlockType type);
extern bool chunk_contains_block(struct Chunk* chunk, vec3 position);
extern void chunk_create_map(struct Chunk* chunk);

#endif // CHUNK_H_

/*
 * NOTE(fonsi): Proceso de Creación, Uso y Destruccion de un CHUNK
 *
 * chunk_init()
 * chunk_create_map()
 * chunk_generate_mesh()
 * chunk_render()
 * chunk_destroy()
 *
 */
