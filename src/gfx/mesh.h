#ifndef MESH_H_
#define MESH_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "../world/block.h"
#include "gfx.h"
#include "renderer.h"
#include "vao.h"
#include "vbo.h"

extern const uint MESH_BUFFER_SIZE_BASE;

// NOTE(fonsi): recordar que el orden de los datos tiene que ser ->
// 1. 3 floats (posicion)
// 2. 2 floats (texturas)

struct Mesh
{
        struct VAO vao;
        struct VBO vbo;
};

extern void mesh_init(struct Mesh* mesh);
extern void mesh_destroy(struct Mesh* mesh);
extern void mesh_add_face(struct Mesh* mesh, vec3 chunk_block_pos, vec2 face_texture_coords, enum Direction direction);
extern void mesh_prepare_render(struct Mesh* mesh);
extern void mesh_render(struct Mesh* mesh);

#endif // MESH_H_
