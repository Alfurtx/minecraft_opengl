#ifndef WORLDGEN_H_
#define WORLDGEN_H_

#define HEIGHTMAP_X 16
#define HEIGHTMAP_Z 16
#include "utils.h"

struct Heightmap
{
        fnl_state noise_state;
        float elevation[HEIGHTMAP_Z][HEIGHTMAP_X];
};

void heightmap_generate(struct Heightmap* heightmap);

#endif // WORLDGEN_H_
