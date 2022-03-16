#include "mesh.h"
#include <stdlib.h>

void
mesh_init(struct Mesh* mesh)
{
}

void
mesh_destroy(struct Mesh* mesh)
{
}

void
mesh_add_face(struct Mesh* mesh, vec3 position, vec2 texture_position, enum Direction direction)
{
}

void
mesh_finalize(struct Mesh* mesh)
{
}

void
mesh_prepare(struct Mesh* mesh)
{
}

void
mesh_render(struct Mesh* mesh)
{
        vao_bind(&mesh->vao);
        vbo_bind(&mesh->vbo);
        vao_attr(&mesh->vao, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
        vao_attr(&mesh->vao, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

        vao_bind(&mesh->vao);
        vbo_bind(&mesh->ibo);
        // glDrawElements(GL_TRIANGLES, mesh->indices.count, GL_UNSIGNED_INT, NULL);
}
