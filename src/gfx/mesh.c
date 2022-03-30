#include "mesh.h"
#include <stdlib.h>

#define DATA_BUFFER_SIZE ((32 * 32 * 32) * 8 * 5 * sizeof(float))
#define INDICES_BUFFER_SIZE ((32 * 32 * 32) * 6 * 6 * sizeof(uint))

// clang-format off
const uint UNIQUE[] = {1, 0, 5, 2};
const float VERTICES[] = {
    0, 0, 0, // 0
    1, 0, 0, // 1
    1, 1, 0, // 2
    0, 1, 0, // 3
    0, 0, 1, // 4
    1, 0, 1, // 5
    1, 1, 1, // 6
    0, 1, 1  // 7
};
const uint INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};
const uint TEXTURES[] = {
    0, 1, // 0
    1, 1, // 1
    1, 0, // 2
    0, 0  // 3
};
// clang-format on

struct {
        float data[DATA_BUFFER_SIZE];
        uint  indices[INDICES_BUFFER_SIZE];
} GLOBAL_BUFFERS;

void
mesh_init(struct Mesh* mesh)
{
        memset(mesh, 0, sizeof(struct Mesh));
        mesh->vao              = vao_create();
        mesh->vbo              = vbo_create(GL_ARRAY_BUFFER, false);
        mesh->ibo              = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);
        mesh->data.capacity    = DATA_BUFFER_SIZE;
        mesh->indices.capacity = INDICES_BUFFER_SIZE;
}

void
mesh_prepare(struct Mesh* mesh)
{
        mesh->data.data     = GLOBAL_BUFFERS.data;
        mesh->indices.data  = GLOBAL_BUFFERS.indices;
        mesh->data.index    = 0;
        mesh->indices.index = 0;
        mesh->data.count    = 0;
        mesh->indices.count = 0;
}

void
mesh_add_face(struct Mesh* mesh, vec3 position, vec2 texture_position, enum Direction direction)
{
        mesh_prepare(mesh);

        float scale = (float) 1 / 16;

        float* data    = mesh->data.data;
        uint*  indices = mesh->indices.data;

        for (uint i = 0; i < 4; i++) {
                const float* vertices    = &VERTICES[INDICES[direction * 6 + UNIQUE[i]]];
                data[mesh->data.index++] = position[0] + vertices[0];
                data[mesh->data.index++] = position[1] + vertices[1];
                data[mesh->data.index++] = position[2] + vertices[2];
                data[mesh->data.index++] = (texture_position[0] + TEXTURES[i]) * scale;
                data[mesh->data.index++] = (texture_position[1] + TEXTURES[i + 1]) * scale;
        }

        for (uint i = 0; i < 6; i++)
                indices[mesh->indices.index++] = mesh->vertex_count + INDICES[direction * 6 + i];

        mesh->vertex_count += 4;

        mesh_finalize(mesh);
}

void
mesh_finalize(struct Mesh* mesh)
{
        mesh->data.count    = mesh->data.index;
        mesh->indices.count = mesh->indices.index;
        mesh->data.index    = 0;
        mesh->indices.index = 0;

        vao_bind(&mesh->vao);
        vbo_bind(&mesh->ibo);
        vbo_buffer(&mesh->ibo, mesh->indices.data, mesh->indices.count * sizeof(uint));
        vbo_bind(&mesh->vbo);
        vbo_buffer(&mesh->vbo, mesh->data.data, mesh->data.count * sizeof(float));
        vao_attr(&mesh->vao, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
        vao_attr(&mesh->vao, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                 (void*) (3 * sizeof(float)));
}

void
mesh_render(struct Mesh* mesh)
{
        vao_bind(&mesh->vao);
        glDrawElements(GL_TRIANGLES, mesh->indices.count, GL_UNSIGNED_INT, 0);
}

void
mesh_destroy(struct Mesh* mesh)
{
        vao_destroy(&mesh->vao);
        vbo_destroy(&mesh->vbo);
        vbo_destroy(&mesh->ibo);
}
