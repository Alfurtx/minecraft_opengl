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

struct
{
        float data[DATA_BUFFER_SIZE];
        uint  indices[INDICES_BUFFER_SIZE];
} GLOBAL_BUFFERS;

void
mesh_init(struct Mesh* mesh)
{
        memset(mesh, 0, sizeof(struct Mesh));
        mesh->vao = vao_create();
        mesh->vbo = vbo_create(GL_ARRAY_BUFFER, false);
        mesh->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);
}

void
mesh_prepare(struct Mesh* mesh)
{
        mesh->data             = GLOBAL_BUFFERS.data;
        mesh->indices          = GLOBAL_BUFFERS.indices;
        mesh->data_capacity    = DATA_BUFFER_SIZE;
        mesh->indices_capacity = INDICES_BUFFER_SIZE;
        mesh->data_count       = 0;
        mesh->indices_count    = 0;
        mesh->data_index       = 0;
        mesh->indices_index    = 0;
        mesh->vertex_count     = 0;
}

void
mesh_add_face(struct Mesh* mesh, vec3 position, vec2 texture_position, enum Direction direction)
{
        mesh_prepare(mesh);

        float scale = 1 / 16;

        // TODO(fonsi): encontrar la manera de transformar los datos de position y texture_position en
        // coordenadas reales, tanto para los vertices como para los indices
        for (uint i = 0; i < 4; i++)
        {
                const float* vertices          = &VERTICES[INDICES[direction * 6 + UNIQUE[i]]];
                mesh->data[mesh->data_index++] = position[0] + vertices[0];
                mesh->data[mesh->data_index++] = position[1] + vertices[1];
                mesh->data[mesh->data_index++] = position[2] + vertices[2];
                mesh->data[mesh->data_index++] = (texture_position[0] + TEXTURES[i]) * scale;
                mesh->data[mesh->data_index++] = (texture_position[1] + TEXTURES[i + 1]) * scale;
        }

        for (uint i = 0; i < 6; i++)
                mesh->indices[mesh->indices_index++] = mesh->vertex_count + INDICES[direction * 6 + i];

        mesh->vertex_count += 4;

        mesh_finalize(mesh);
}

void
mesh_finalize(struct Mesh* mesh)
{
        mesh->data_count    = mesh->data_index;
        mesh->indices_count = mesh->indices_index;
        mesh->data_index    = 0;
        mesh->indices_index = 0;

        vbo_bind(&mesh->vbo);
        vbo_buffer(&mesh->vbo, mesh->data, mesh->data_count * sizeof(float));
        vbo_bind(&mesh->ibo);
        vbo_buffer(&mesh->ibo, mesh->indices, mesh->indices_count * sizeof(uint));
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
        glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, NULL);
}

void
mesh_destroy(struct Mesh* mesh)
{
        vao_destroy(&mesh->vao);
        vbo_destroy(&mesh->vbo);
        vbo_destroy(&mesh->ibo);
}
