#include "utils.h"
#include "worldgen.h"
#include "types.h"

internal float
noise(fnl_state* state, float x, float y)
{
        return fnlGetNoise2D(state, x, y) / 2.0 + 0.5;
}

void
heightmap_generate(struct Heightmap* heightmap)
{
        for (uint x = 0; x < HEIGHTMAP_X; x++)
                for (uint z = 0; z < HEIGHTMAP_Z; z++)
                {
                        double nx = (double) x / HEIGHTMAP_X - 0.5;
                        double nz = (double) z / HEIGHTMAP_Z - 0.5;

                        heightmap->elevation[z][x] = round(noise(&heightmap->noise_state, nx, nz) * 18) / 18;
                }
}
