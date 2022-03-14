#include "block.h"

struct Block BLOCKS[BLOCK_TYPE_COUNT];
struct Block BLOCK_DEFAULT;

internal void
grass_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
                glm_vec2_copy((vec2){0, 0}, dest);
                break;
        case DOWN:
                glm_vec2_copy((vec2){2, 0}, dest);
                break;
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){1, 0}, dest);
                break;
        }
}

internal void
grass_init()
{
        BLOCKS[BLOCK_GRASS].active               = true;
        BLOCKS[BLOCK_GRASS].type                 = BLOCK_GRASS;
        BLOCKS[BLOCK_GRASS].get_texture_location = grass_get_texture_location;
}

internal void
sand_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){0, 1}, dest);
                break;
        }
}

internal void
sand_init()
{
        BLOCKS[BLOCK_SAND].active               = true;
        BLOCKS[BLOCK_SAND].type                 = BLOCK_SAND;
        BLOCKS[BLOCK_SAND].get_texture_location = sand_get_texture_location;
}

internal void
snow_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){1, 1}, dest);
                break;
        }
}

internal void
snow_init()
{
        BLOCKS[BLOCK_GLASS].active               = true;
        BLOCKS[BLOCK_GLASS].type                 = BLOCK_GLASS;
        BLOCKS[BLOCK_GLASS].get_texture_location = snow_get_texture_location;
}

internal void
water_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){0, 2}, dest);
                break;
        }
}

internal void
water_init()
{
        BLOCKS[BLOCK_WATER].active               = true;
        BLOCKS[BLOCK_WATER].type                 = BLOCK_WATER;
        BLOCKS[BLOCK_WATER].get_texture_location = water_get_texture_location;
}

internal void
clay_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){5, 1}, dest);
                break;
        }
}

internal void
clay_init()
{
        BLOCKS[BLOCK_CLAY].active               = true;
        BLOCKS[BLOCK_CLAY].type                 = BLOCK_CLAY;
        BLOCKS[BLOCK_CLAY].get_texture_location = clay_get_texture_location;
}

internal void
gravel_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){6, 0}, dest);
                break;
        }
}

internal void
gravel_init()
{
        BLOCKS[BLOCK_GRAVEL].active               = true;
        BLOCKS[BLOCK_GRAVEL].type                 = BLOCK_GRAVEL;
        BLOCKS[BLOCK_GRAVEL].get_texture_location = gravel_get_texture_location;
}

internal void
dirt_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){2, 0}, dest);
                break;
        }
}

internal void
dirt_init()
{
        BLOCKS[BLOCK_DIRT].active               = true;
        BLOCKS[BLOCK_DIRT].type                 = BLOCK_DIRT;
        BLOCKS[BLOCK_DIRT].get_texture_location = dirt_get_texture_location;
}

internal void
stone_get_texture_location(enum Direction direction, vec2 dest)
{
        switch (direction)
        {
        case UP:
        case DOWN:
        case EAST:
        case WEST:
        case NORTH:
        case SOUTH:
                glm_vec2_copy((vec2){3, 0}, dest);
                break;
        }
}

internal void
stone_init()
{
        BLOCKS[BLOCK_STONE].active               = true;
        BLOCKS[BLOCK_STONE].type                 = BLOCK_STONE;
        BLOCKS[BLOCK_STONE].get_texture_location = stone_get_texture_location;
}

internal void
air_init()
{
        BLOCKS[BLOCK_AIR].active = false;
        BLOCKS[BLOCK_AIR].type   = BLOCK_AIR;
}

void
block_list_init()
{
        grass_init();
        stone_init();
        sand_init();
        snow_init();
        water_init();
        clay_init();
        gravel_init();
        dirt_init();
        air_init();

        BLOCK_DEFAULT        = BLOCKS[BLOCK_STONE];
        BLOCK_DEFAULT.active = false;
}

void
block_init(struct Block* block)
{
        *block = BLOCK_DEFAULT;
}
