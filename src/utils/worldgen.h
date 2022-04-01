#ifndef WORLDGEN_H_
#define WORLDGEN_H_

#define HEIGHTMAP_X 32
#define HEIGHTMAP_Z 32
#include "utils.h"
#include "../world/chunk.h"

struct Heightmap {
        fnl_state noise_state;
        float     elevation[HEIGHTMAP_Z][HEIGHTMAP_X];
};

void heightmap_init(struct Heightmap* heightmap);
void heightmap_generate(struct Heightmap* heightmap, float chunk_x, float chunk_z);
void heightmap_generate_seed(struct Heightmap* heightmap, int x, int y);
extern void worldgen_generate(struct Chunk* chunk);

#endif // WORLDGEN_H_
