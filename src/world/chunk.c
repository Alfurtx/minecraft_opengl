#include "../state.h"
#include "../utils/utils.h"
#include "chunk.h"
#include "world.h"

// TODO(fonsi): implementar generacion de mundo apropiada
internal void        chunk_setup_map(struct Chunk* chunk);
internal inline uint offset(uint x, uint y, uint z);
internal inline void offset3d(uint offset, vec3 dest);
internal inline void set_block_active(uint* block, bool active);
internal inline bool get_block_active(uint block);
internal inline void set_block_type(uint* block, enum BlockType type);
internal inline uint get_block_type(uint block);

#define FOR_EACH_BLOCK_XYZ(i, j, k)                     \
        for (uint i = 0; i < CHUNK_SIZE_X; i++)         \
                for (uint j = 0; j < CHUNK_SIZE_Y; j++) \
                        for (uint k = 0; k < CHUNK_SIZE_Z; k++)

struct Chunk*
chunk_init(struct Renderer* renderer, vec3 world_position, fnl_state* noise_state)
{
        struct Chunk* chunk = malloc(sizeof *chunk);
        chunk->renderer     = renderer;

        chunk->prepared = false;
        chunk->border   = false;

        chunk->xn = NULL;
        chunk->xp = NULL;
        chunk->zn = NULL;
        chunk->zp = NULL;

        mesh_init(&chunk->mesh);
        glm_vec3_copy(world_position, chunk->world_position);
        glm_vec3_scale(world_position, 16.0f, chunk->world_offset);

        // chunk->heightmap.noise_state = noise_state;

        for (uint i = 0; i < CHUNK_BLOCK_COUNT; i++)
                chunk->blocks[i] = 0;

        chunk_setup_map(chunk);

        return (chunk);
}

void
chunk_update(struct Chunk* chunk)
{
}

void
chunk_destroy(struct Chunk* chunk)
{
        mesh_destroy(&chunk->mesh);
        free(chunk);
        chunk = NULL;
}

void
chunk_prepare_render(struct Chunk* chunk)
{
        FOR_EACH_BLOCK_XYZ(i, j, k)
        {
                uint current_block = chunk->blocks[offset(i, j, k)];
                if (current_block & BLOCK_MASK_ACTIVE)
                {
                        // ir cara por cara de cada bloque
                        for (uint d = 0; d < DIRECTION_COUNT; d++)
                        {
                                // conseguir el bloque vecino
                                vec3 aux;
                                glm_vec3_add((vec3){i, j, k}, DIRECTION_VEC[d], aux);

                                // comprobar que si bloque adyacente no esta activo
                                if (!world_block_exists(&state.world, chunk, aux))
                                {
                                        // pasarlo al mesh
                                        vec2 texture_coords;
                                        BLOCKS[get_block_type(current_block)].get_texture_location(d, texture_coords);
                                        mesh_add_face(&chunk->mesh, (vec3){i, j, k}, texture_coords, d);
                                }
                        }
                }
        }
        mesh_prepare_render(&chunk->mesh);
}

void
chunk_render(struct Chunk* chunk)
{
        renderer_set_type(chunk->renderer, RENDERER_CHUNK);
        mat4 view       = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        mat4 model      = GLM_MAT4_IDENTITY_INIT;
        glm_translate_x(model, chunk->world_offset[0]);
        glm_translate_y(model, chunk->world_offset[1]);
        glm_translate_z(model, chunk->world_offset[2]);
        camera_get_view(chunk->renderer->current_camera, view);
        camera_get_projection(chunk->renderer->current_camera, projection);
        shader_set_uniform_mat4(chunk->renderer->current_shader, "projection", 1, GL_FALSE, projection[0]);
        shader_set_uniform_mat4(chunk->renderer->current_shader, "view", 1, GL_FALSE, view[0]);
        shader_set_uniform_mat4(chunk->renderer->current_shader, "model", 1, GL_FALSE, model[0]);
        mesh_render(&chunk->mesh);
}


internal inline uint
offset(uint x, uint y, uint z)
{
        return (x + CHUNK_SIZE_X * y + CHUNK_SIZE_X * CHUNK_SIZE_Y * z);
}

internal inline void
offset3d(uint offset, vec3 dest)
{
        dest[2] = offset / (CHUNK_SIZE_X * CHUNK_SIZE_Y);
        offset -= (dest[2] * CHUNK_SIZE_X * CHUNK_SIZE_Y);
        dest[1] = offset / CHUNK_SIZE_X;
        dest[0] = offset % CHUNK_SIZE_X;
}

internal void
chunk_setup_map(struct Chunk* chunk)
{
        // chunk->heightmap.noise_state              = fnlCreateState();
        // chunk->heightmap.noise_state.noise_type   = FNL_NOISE_OPENSIMPLEX2;
        // chunk->heightmap.noise_state.octaves      = 16;
        // chunk->heightmap.noise_state.lacunarity   = 4.0;
        // chunk->heightmap.noise_state.frequency    = 0.01;
        // chunk->heightmap.noise_state.fractal_type = FNL_FRACTAL_FBM;
        // chunk->heightmap.noise_state.seed = chunk->world_offset[0] + chunk->world_offset[1] + chunk->world_offset[2];

        // heightmap_generate(&chunk->heightmap);

        // for (uint i = 0; i < HEIGHTMAP_X; i++)
        //         for (uint j = 0; j < HEIGHTMAP_Z; j++)
        //         {
        //                 double height                       = 55.0f * (double) chunk->heightmap.elevation[i][j];
        //                 chunk->blocks[offset(i, height, j)] = BLOCKS[BLOCK_GRASS];
        //                 if (height != 0)
        //                         for (uint k = height - 1; k > 0; k--)
        //                                 chunk->blocks[offset(i, k, j)] = BLOCKS[BLOCK_STONE];
        //         }

        for (uint i = 0; i < CHUNK_SIZE_X; i++)
                for (uint j = 0; j < 16; j++)
                        for (uint k = 0; k < CHUNK_SIZE_Z; k++)
                        {
                                uint* block = &chunk->blocks[offset(i, j, k)];
                                *block      = *block | 0x01;
                                *block      = (*block & BLOCK_MASK_TYPE) | (((uint) BLOCK_STONE) << 8);
                        }
}

internal inline void
set_block_active(uint* block, bool active)
{
        *block = active ? *block | 0x01 : *block & BLOCK_MASK_ACTIVE;
}

internal inline bool
get_block_active(uint block)
{
        return block & 0x000000ff;
}

internal inline void
set_block_type(uint* block, enum BlockType type)
{
        uint aux = type;
        *block   = (*block & BLOCK_MASK_TYPE) | (aux << 8);
}

internal inline uint
get_block_type(uint block)
{
        return (block & BLOCK_MASK_TYPE) >> 8;
}
