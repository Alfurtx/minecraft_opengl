#include "worldgen.h"
#include "types.h"
#include "utils.h"

// NOTE(fonsi): esto esta completamente copiado de jdh ya que no tengo energia ni ganas para lidiar
// con la mierda esta

#define SRAND(seed) srand(seed)
#define RAND(min, max) ((rand() % (max - min + 1)) + min)
#define RANDCHANCE(chance) ((RAND(0, 100000) / 100000.0) <= chance)

#define WATER_LEVEL 4

int64_t
ivec3shash(vec3 v)
{
        int64_t h = 0;
        for (int i = 0; i < 3; i++) {
                h ^= *(int64_t*) &v[i] + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
}
typedef void (*FSet)(struct Chunk*, int, int, int, uint);
typedef uint (*FGet)(struct Chunk*, int, int, int);
typedef float (*FNoise)(void* p, float s, float x, float z);

struct Noise {
        uint8  params[512];
        FNoise compute;
};
struct Octave {
        int n, o;
};
struct Combined {
        struct Noise *n, *m;
};

float
octave_compute(struct Octave* p, float seed, float x, float z)
{
        float u = 1.0f, v = 0.0f;
        for (int i = 0; i < p->n; i++) {
                v += noise3(x / u, z / u, seed + i + (p->o * 32)) * u;
                u *= 2.0f;
        }
        return v;
}

struct Noise
octave(int n, int o)
{
        struct Noise  result = {.compute = (FNoise) octave_compute};
        struct Octave params = {n, o};
        memcpy(&result.params, &params, sizeof(struct Octave));
        return result;
}

float
combined_compute(struct Combined* p, float seed, float x, float z)
{
        return p->n->compute(&p->n->params, seed, x + p->m->compute(&p->m->params, seed, x, z), z);
}

struct Noise
combined(struct Noise* n, struct Noise* m)
{
        struct Noise    result = {.compute = (FNoise) combined_compute};
        struct Combined params = {n, m};
        memcpy(&result.params, &params, sizeof(struct Combined));
        return result;
}

void
worldgen_generate(struct Chunk* chunk)
{
        // TODO: configure in world.c
        const uint64 seed = 2;
        SRAND(seed + ivec3shash(chunk->offset));

        // biome noise
        struct Noise n = octave(6, 0);

        // ore noise
        struct Noise m = octave(6, 1);

        // Different offsets of octave noise functions
        struct Noise os[] = {
            octave(8, 1), octave(8, 2), octave(8, 3), octave(8, 4), octave(8, 5), octave(8, 6),
        };

        // Two separate combined noise functions, each combining two different
        // octave noise functions
        struct Noise cs[] = {
            combined(&os[0], &os[1]),
            combined(&os[2], &os[3]),
            combined(&os[4], &os[5]),
        };

        for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                        int wx = chunk->offset[0] + x, wz = chunk->offset[2] + z;

                        // Sample each combined noise function for high/low results
                        const float base_scale = 1.3f;
                        int         hr;
                        int         hl =
                            (cs[0].compute(&cs[0].params, seed, wx * base_scale, wz * base_scale) /
                             6.0f) -
                            4.0f;
                        int hh =
                            (cs[1].compute(&cs[1].params, seed, wx * base_scale, wz * base_scale) /
                             5.0f) +
                            6.0f;

                        // Sample the biome noise and extra noise
                        float t = n.compute(&n.params, seed, wx, wz);
                        float r = m.compute(&m.params, seed, wx / 4.0f, wz / 4.0f) / 32.0f;

                        if (t > 0) {
                                hr = hl;
                        } else {
                                hr = hh > hl ? hh : hl;
                        }

                        // offset by water level and determine biome
                        // int h = hr + WATER_LEVEL;
                        int h = hr + WATER_LEVEL;

                        // beach is anything close-ish to water in biome AND height
                        // enum Biome biome;
                        // if (h < WATER_LEVEL) {
                        //     biome = OCEAN;
                        // } else if (t < 0.08f && h < WATER_LEVEL + 2) {
                        //     biome = BEACH;
                        // } else if (false) {
                        //     biome = MOUNTAIN;
                        // } else {
                        //     biome = PLAINS;
                        // }

                        // if (biome == MOUNTAIN) {
                        //     h += (r + (-t / 12.0f)) * 2 + 2;
                        // }

                        // // dirt or sand depth
                        // int d = r * 1.4f + 5.0f;

                        // enum BlockId top_block;
                        // switch (biome) {
                        //     case OCEAN:
                        //         if (r > 0.8f) {
                        //             top_block = GRAVEL;
                        //         } else if (r > 0.3f) {
                        //             top_block = SAND;
                        //         } else if (r > 0.15f && t < 0.08f) {
                        //             top_block = CLAY;
                        //         } else {
                        //             top_block = DIRT;
                        //         }
                        //         break;
                        //     case BEACH:
                        //         top_block = SAND;
                        //         break;
                        //     case PLAINS:
                        //         top_block = (t > 4.0f && r > 0.78f) ? GRAVEL : GRASS;
                        //         break;
                        //     case MOUNTAIN:
                        //         if (r > 0.8f) {
                        //             top_block = GRAVEL;
                        //         } else if (r > 0.7f) {
                        //             top_block = DIRT;
                        //         } else {
                        //             top_block = STONE;
                        //         }
                        //         break;
                        // }

                        chunk_set_block(chunk, (vec3){x, h, z}, BLOCK_GRASS);
                        for (int y = 0; y < h; y++) {
                                //     enum BlockId block;
                                //     if (y == (h - 1)) {
                                //         block = top_block;
                                //     } else if (y > (h - d)) {
                                //         if (top_block == GRASS) {
                                //             block = DIRT;
                                //         } else {
                                //             block = top_block;
                                //         }
                                //     } else {
                                //         block = STONE;
                                //     }

                                chunk_set_block(chunk, (vec3){x, y, z}, BLOCK_STONE);
                        }
                        for (int y = h; y < WATER_LEVEL; y++) {
                                chunk_set_block(chunk, (vec3){x, y, z}, BLOCK_WATER);
                        }
                }
        }
}
