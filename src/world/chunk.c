#include "../state.h"
#include "../utils/utils.h"
#include "chunk.h"
#include "world.h"

// TODO(fonsi): implementar generacion de mundo apropiada
internal void chunk_setup_map(struct Chunk* chunk);
internal uint offset(uint x, uint y, uint z);
internal void offset3d(uint offset, vec3 dest);

#define FOR_EACH_BLOCK(ptr, func)                    \
        for (uint i = 0; i < CHUNK_BLOCK_COUNT; i++) \
                func(&ptr->blocks[i]);

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

        mesh_init(&chunk->mesh);
        glm_vec3_copy(world_position, chunk->world_position);
        glm_vec3_scale(world_position, 16.0f, chunk->world_offset);

        // chunk->heightmap.noise_state = noise_state;

        FOR_EACH_BLOCK(chunk, block_init)

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
                struct Block current_block = chunk->blocks[offset(i, j, k)];
                if (current_block.active)
                {
                        // ir cara por cara de cada bloque
                        for (uint d = 0; d < DIRECTION_COUNT; d++)
                        {
                                // conseguir el bloque vecino
                                vec3 aux;
                                glm_vec3_add((vec3){i, j, k}, DIRECTION_VEC[d], aux);
                                struct Block neighbor_block = world_get_block(&state.world, chunk->world_position, aux);

                                // comprobar que si bloque adyacente no esta activo
                                if (!neighbor_block.active)
                                {
                                        // pasarlo al mesh
                                        vec2 texture_coords;
                                        current_block.get_texture_location(d, texture_coords);
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

// TODO(fonsi): Terminar la funcion, basandome en la funcion que vi en este repo
// (https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33)
void
chunk_generate_mesh(struct Chunk* chunk)
{
        for (uint d = 0; d < 3; d++)
        {
                int i, j, k, l, w, h;
                int u = (d + 1) % 3;
                int v = (d + 2) % 3;
                int x[3];
                int q[3];

                bool mask[CHUNK_SIZE_X * CHUNK_SIZE_Z];
                q[d] = 1;

                for (x[d] = -1; x[d] < CHUNK_SIZE_X;)
                {
                        int n = 0;
                        for (x[v] = 0; x[v] < CHUNK_SIZE_X; x[v]++)
                        {
                                for (x[u] = 0; x[u] < CHUNK_SIZE_X; x[u]++)
                                {
                                        bool block_current =
                                            0 <= x[d] ? world_is_block_at(&state.world,
                                                                          chunk->world_position,
                                                                          (vec3){x[0] + chunk->world_position[0],
                                                                                 x[1] + chunk->world_position[1],
                                                                                 x[2] + chunk->world_position[2]})
                                                      : true;

                                        bool block_compare =
                                            x[d] < CHUNK_SIZE_X - 1
                                                ? world_is_block_at(&state.world,
                                                                    chunk->world_position,
                                                                    (vec3){x[0] + q[0] + chunk->world_position[0],
                                                                           x[1] + q[1] + chunk->world_position[1],
                                                                           x[2] + q[2] + chunk->world_position[2]})
                                                : true;

                                        mask[n++] = block_compare != block_current;
                                }
                        }
                }
        }
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

// TODO(fonsi): cuantos mas bloques mas tarda, F
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
                for (uint j = 0; j < 55; j++)
                        for (uint k = 0; k < CHUNK_SIZE_Z; k++)
                                chunk->blocks[offset(i, j, k)] = BLOCKS[BLOCK_GRASS];
}
