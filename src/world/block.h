#ifndef BLOCK_H_
#define BLOCK_H_

#include "../utils/types.h"
#include "../utils/utils.h"

enum BlockType
{
        BLOCK_AIR = 0,
        BLOCK_GRASS,
        BLOCK_STONE,
        BLOCK_SAND,
        BLOCK_GLASS,
        BLOCK_WATER,
        BLOCK_CLAY,
        BLOCK_GRAVEL,
        BLOCK_DIRT,
        BLOCK_TYPE_COUNT,
};

struct Block
{
        bool           active;
        enum BlockType type;
        void (*get_texture_location)(enum Direction direction, vec2 dest);
};

extern struct Block BLOCK_DEFAULT;
extern struct Block BLOCKS[BLOCK_TYPE_COUNT];

extern void block_list_init();
extern void block_init(struct Block* block);

#endif // BLOCK_H_
