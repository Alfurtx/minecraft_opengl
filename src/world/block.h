#ifndef BLOCK_H_
#define BLOCK_H_

#include "../utils/types.h"
#include "../utils/utils.h"

enum BlockType
{
        BLOCK_GRASS = 0,
        BLOCK_STONE,
        BLOCK_SAND,
        BLOCK_SNOW,
        BLOCK_WATER,
        BLOCK_CLAY,
        BLOCK_GRAVEL,
        BLOCK_DIRT,
        BLOCK_TYPE_COUNT,
};

struct Block
{
        bool active;
        enum BlockType type;
        void (* get_texture_location)(enum Direction direction, vec2 dest);
};

extern struct Block BLOCKS[BLOCK_TYPE_COUNT];

extern void block_init();

#endif // BLOCK_H_
