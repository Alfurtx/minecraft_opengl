#include "mesh.h"
#include <stdlib.h>

// clang-format off
const uint MESH_FACE_INDICES[] = {1, 0, 3, 1, 3, 2};
const uint MESH_UNIQUE_INDICES[] = {1, 0, 5, 2};
const uint MESH_INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};

const float MESH_VERTICES[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,

    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
};

const float MESH_TEXTURE_COORDS[] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1
};
// clang-format on

#define BUFFER_DATA_SIZE ((32 * 32 * 32) * 8 * 5 * sizeof(float))
#define BUFFER_INDICES_SIZE ((32 * 32 * 32) * 36 * sizeof(uint))

struct
{
        float data[BUFFER_DATA_SIZE];
        uint  indices[BUFFER_INDICES_SIZE];
} GLOBAL_BUFFERS;

void
mesh_init(struct Mesh* mesh)
{
        memset(mesh, 0, sizeof(struct Mesh));
        mesh->vao              = vao_create();
        mesh->vbo              = vbo_create(GL_ARRAY_BUFFER, false);
        mesh->ibo              = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);
        mesh->data.capacity    = BUFFER_DATA_SIZE;
        mesh->indices.capacity = BUFFER_INDICES_SIZE;
}

void
mesh_destroy(struct Mesh* mesh)
{
        vao_destroy(&mesh->vao);
        vbo_destroy(&mesh->vbo);
        vbo_destroy(&mesh->ibo);
}

void
mesh_add_face(struct Mesh* mesh, vec3 position, vec2 texture_position, enum Direction direction)
{
        float texture_scale = 1.0f / 16.0f;

        for (uint i = 0; i < 4; i++)
        {
                const float* aux = &MESH_VERTICES[MESH_INDICES[direction * 6 + MESH_UNIQUE_INDICES[i]] * 3];

                ((float*) mesh->data.data)[mesh->data.index++] = position[0] + aux[0];
                ((float*) mesh->data.data)[mesh->data.index++] = position[1] + aux[1];
                ((float*) mesh->data.data)[mesh->data.index++] = position[2] + aux[2];
                ((float*) mesh->data.data)[mesh->data.index++] =
                    texture_position[0] * texture_scale + (texture_scale * MESH_TEXTURE_COORDS[i * 2 + 0]);
                ((float*) mesh->data.data)[mesh->data.index++] =
                    texture_position[1] * texture_scale + (texture_scale * MESH_TEXTURE_COORDS[i * 2 + 1]);
        }

        for (uint i = 0; i < 6; i++)
                ((uint*) mesh->indices.data)[mesh->indices.index++] = mesh->vertex_count + MESH_FACE_INDICES[i];

        mesh->vertex_count += 4;
}

void
mesh_finalize(struct Mesh* mesh)
{
        mesh->data.count    = mesh->data.index;
        mesh->data.index    = 0;
        mesh->indices.count = mesh->indices.index;
        mesh->indices.index = 0;

        vbo_buffer(&mesh->vbo, mesh->data.data, mesh->data.count * sizeof(float));
        vbo_buffer(&mesh->ibo, mesh->indices.data, mesh->indices.count * sizeof(uint));
}

void
mesh_prepare(struct Mesh* mesh)
{
        mesh->vertex_count = 0;
        mesh->data.data    = GLOBAL_BUFFERS.data;
        mesh->indices.data = GLOBAL_BUFFERS.indices;
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
        glDrawElements(GL_TRIANGLES, mesh->indices.count, GL_UNSIGNED_INT, NULL);
        vao_bind(0);
}
