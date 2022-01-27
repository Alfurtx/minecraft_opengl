#include "chunk.h"

// TODO(fonsi): implementar generacion de mundo apropiada
internal void chunk_setup_map(struct Chunk* chunk);
internal uint offset(uint x, uint y, uint z);
internal void offset3d(uint offset, vec3 dest);

void
chunk_init(struct Chunk* chunk, vec3 world_offset)
{
        glm_vec3_copy(world_offset, chunk->world_offset);
}

void
chunk_update(struct Chunk* chunk)
{
}

void
chunk_render(struct Chunk* chunk)
{
}

internal uint
offset(uint x, uint y, uint z)
{
        return (x + CHUNK_SIZE_X * y + CHUNK_SIZE_X * CHUNK_SIZE_Y * z);
}

internal void
offset3d(uint offset, vec3 dest)
{
        dest[2] = offset / (CHUNK_SIZE_X * CHUNK_SIZE_Y);
        offset -= (dest[2] * CHUNK_SIZE_X * CHUNK_SIZE_Y);
        dest[1] = offset / CHUNK_SIZE_X;
        dest[0] = offset % CHUNK_SIZE_X;
}

internal void chunk_setup_map(struct Chunk* chunk)
{
        chunk->blocks[offset(0, 0, 0)] = BLOCKS[BLOCK_GRASS];
}
