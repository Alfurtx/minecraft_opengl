#include "world.h"

#define WORLDOFFSET(a, b) ((int) (a * 16 + b))
#define CHUNKOFFSET(vec) ((int) (vec[0] + CHUNK_SIZE_X * vec[1] + CHUNK_SIZE_X * CHUNK_SIZE_Y * vec[2]))
#define CHUNKOFFSET3D(a, b, c) ((int) (a + CHUNK_SIZE_X * b + CHUNK_SIZE_X * CHUNK_SIZE_Y * c))

void
world_init(struct World* world, struct Renderer* renderer)
{
        world->renderer = renderer;
        block_init();

        // TODO(fonsi): hacer sistema de mundo infinito y no tan estatico
        for (uint i = 0; i < 16; i++)
                for (uint j = 0; j < 16; j++)
                        chunk_init(&world->chunks[i * 16 + j], renderer, (vec3){j * 16, 0, i * 16});
}

void
world_destroy(struct World* world)
{
}

void
world_update(struct World* world)
{
}

void
world_render(struct World* world)
{
        for (uint i = 0; i < WORLD_CHUNK_COUNT; i++)
                chunk_render(&world->chunks[i]);
}

struct Block
world_get_block(struct World* world, vec3 chunk_world_offset, vec3 chunk_block_position, enum Direction direction)
{
        if (glm_vec3_max(chunk_block_position) < 16 && glm_vec3_min(chunk_block_position) > -1)
        {
                struct Chunk chunk = world_get_chunk(world, chunk_world_offset);
                return chunk.blocks[CHUNKOFFSET(chunk_block_position)];
        }
        else
        {
                vec3 aux;
                glm_vec3_copy(aux, DIRECTIONVEC(direction));
                glm_vec3_add(chunk_world_offset, aux, aux);
                struct Chunk chunk = world_get_chunk(world, aux);
                switch (direction)
                {
                case NORTH:
                        return chunk.blocks[CHUNKOFFSET3D(chunk_block_position[0], chunk_block_position[1], 15)];
                        break;
                case SOUTH:
                        return chunk.blocks[CHUNKOFFSET3D(chunk_block_position[0], chunk_block_position[1], 0)];
                        break;
                case EAST:
                        return chunk.blocks[CHUNKOFFSET3D(0, chunk_block_position[1], chunk_block_position[2])];
                        break;
                case WEST:
                        return chunk.blocks[CHUNKOFFSET3D(15, chunk_block_position[1], chunk_block_position[2])];
                        break;
                default:
                        return BLOCK_DEFAULT;
                        break;
                }
        }
}

struct Chunk
world_get_chunk(struct World* world, vec3 chunk_world_offset)
{
        int offset = WORLDOFFSET(chunk_world_offset[0], chunk_world_offset[1]);
        return (world->chunks[offset]);
}
