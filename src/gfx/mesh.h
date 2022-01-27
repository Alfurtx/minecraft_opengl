#ifndef MESH_H_
#define MESH_H_

#include "../utils/utils.h"
#include "../utils/types.h"
#include "gfx.h"
#include "vao.h"
#include "vbo.h"
#include "renderer.h"

struct Mesh
{
        struct VAO vao;
        struct VBO vbo;
        struct Renderer* renderer;

        void* vertex_buffer_data;
        uint vertex_buffer_size;
        uint vertex_buffer_count;
};

extern void mesh_init(struct Mesh* mesh, struct Renderer* renderer);
extern void mesh_destroy(struct Mesh* mesh);
extern void mesh_add_face(struct Mesh* mesh);

#endif // MESH_H_
