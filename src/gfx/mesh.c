#include "mesh.h"
#include <stdlib.h>

const uint MESH_BLOCK_INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};

const float MESH_BLOCK_VERTICES[][3] = {
    {-0.5f,  0.5f,  0.5f}, // 0
    { 0.5f,  0.5f,  0.5f}, // 1
    { 0.5f, -0.5f,  0.5f}, // 2
    {-0.5f, -0.5f,  0.5f}, // 3
    {-0.5f,  0.5f, -0.5f}, // 4
    { 0.5f,  0.5f, -0.5f}, // 5
    { 0.5f, -0.5f, -0.5f}, // 6
    {-0.5f, -0.5f, -0.5f}, // 7
};

internal void mesh_check_buffer_size(struct Mesh* mesh, uint amount);

void
mesh_init(struct Mesh* mesh)
{
        mesh->vao                 = vao_create();
        mesh->vbo                 = vbo_create(GL_ARRAY_BUFFER, true);
        mesh->vertex_buffer_count = 0;
        mesh->vertex_buffer_data  = malloc(MESH_BUFFER_SIZE_INIT * MESH_VERTEX_BYTE_SIZE * 5);
        mesh->vertex_buffer_size  = MESH_BUFFER_SIZE_INIT;
}

void
mesh_destroy(struct Mesh* mesh)
{
}

void
mesh_add_face(struct Mesh* mesh, vec3 chunk_block_pos, vec2 face_texture_coords, enum Direction direction)
{
        mesh_check_buffer_size(mesh, 1);
}

void
mesh_prepare_render(struct Mesh* mesh)
{
        vao_bind(&mesh->vao);
        vbo_bind(&mesh->vbo);
        vbo_buffer(&mesh->vbo, mesh->vertex_buffer_data, mesh->vertex_buffer_count * sizeof(float));
        vao_attr(&mesh->vao, 0, 3, GL_FLOAT, false, 5 * sizeof(float), 0);
        vao_attr(&mesh->vao, 1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*) (3 * sizeof(float)));
}

void
mesh_render(struct Mesh* mesh)
{
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_buffer_count);
}

internal void
mesh_check_buffer_size(struct Mesh* mesh, uint amount)
{
        if (mesh->vertex_buffer_count + amount >= mesh->vertex_buffer_size)
        {
                mesh->vertex_buffer_data =
                    realloc(mesh->vertex_buffer_data, mesh->vertex_buffer_size * 2 * MESH_VERTEX_BYTE_SIZE);
        }
}
