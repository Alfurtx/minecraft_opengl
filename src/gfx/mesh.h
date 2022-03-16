#ifndef MESH_H_
#define MESH_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "../world/block.h"
#include "gfx.h"
#include "renderer.h"
#include "vao.h"
#include "vbo.h"

// NOTE(fonsi): recordar que el orden de los datos tiene que ser ->
// 1. 3 floats (posicion)
// 2. 2 floats (texturas)

struct Mesh
{
        struct VAO vao;
        struct VBO vbo;
        struct VBO ibo;
};

extern void mesh_init(struct Mesh* mesh);
extern void mesh_destroy(struct Mesh* mesh);
extern void mesh_prepare(struct Mesh* mesh);
extern void mesh_add_face(struct Mesh* mesh, vec3 position, vec2 texture_position, enum Direction direction);
extern void mesh_finalize(struct Mesh* mesh);
extern void mesh_render(struct Mesh* mesh);

#endif // MESH_H_
