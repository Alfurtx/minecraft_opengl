#include "mesh.h"

const uint MESH_BLOCK_INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};

const float MESH_BLOCK_VERTICES[][3] = {
    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
};

void
mesh_init(struct Mesh* mesh, struct Renderer* renderer)
{
}

void
mesh_destroy(struct Mesh* mesh)
{
}

void
mesh_add_face(struct Mesh* mesh)
{
}

void
mesh_prepare_render(struct Mesh* mesh)
{
}

void
mesh_render(struct Mesh* mesh)
{
}
