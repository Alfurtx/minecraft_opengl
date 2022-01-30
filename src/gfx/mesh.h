#ifndef MESH_H_
#define MESH_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "gfx.h"
#include "renderer.h"
#include "vao.h"
#include "vbo.h"

// NOTE(fonsi): el tamaño actual de un vertex es de 5 floats
// 3 floats => posicion | 2 floats => coordenadas en textura
#define MESH_VERTEX_BYTE_SIZE (5 * sizeof(float))
#define MESH_BUFFER_SIZE_INIT 5120

struct Mesh
{
        struct VAO vao;
        struct VBO vbo;

        float* vertex_buffer_data;
        uint  vertex_buffer_size;
        uint  vertex_buffer_count;
};

extern void mesh_init(struct Mesh* mesh);
extern void mesh_destroy(struct Mesh* mesh);
extern void mesh_add_face(struct Mesh* mesh, vec3 chunk_block_pos, vec2 face_texture_coords, enum Direction direction);
extern void mesh_prepare_render(struct Mesh* mesh);
extern void mesh_render(struct Mesh* mesh);

#endif // MESH_H_
