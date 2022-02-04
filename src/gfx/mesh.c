#include "mesh.h"
#include <stdlib.h>

const uint MESH_BUFFER_SIZE_BASE = 25600;

const uint MESH_BLOCK_INDICES[][6] = {
    {0, 1, 3, 1, 2, 3}, // north(-z)
    {7, 5, 4, 7, 6, 5}, // south(+z)
    {1, 5, 2, 5, 6, 2}, // east(+x)
    {3, 4, 0, 3, 7, 4}, // west(-x)
    {4, 5, 0, 5, 1, 0}, // top(+y)
    {3, 6, 7, 3, 2, 6}, // bottom(-y)
};

static vec3 MESH_BLOCK_VERTICES[] = {
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
internal void get_real_texture_coords(vec2 face_text_position, enum Direction direction, uint vertex_index, vec2 dest);

void
mesh_init(struct Mesh* mesh)
{
        mesh->vao                 = vao_create();
        mesh->vbo                 = vbo_create(GL_ARRAY_BUFFER, true);
        mesh->vertex_buffer_count = 0;
        mesh->vertex_buffer_data  = malloc(MESH_BUFFER_SIZE_BASE * sizeof(float));
        mesh->vertex_buffer_size  = MESH_BUFFER_SIZE_BASE;
}

void
mesh_destroy(struct Mesh* mesh)
{
        free(mesh->vertex_buffer_data);
        vao_destroy(&mesh->vao);
        vbo_destroy(&mesh->vbo);
}

void
mesh_add_face(struct Mesh* mesh, vec3 chunk_block_pos, vec2 face_texture_coords, enum Direction direction)
{
        if (mesh->vertex_buffer_count + (5 * 6) >= mesh->vertex_buffer_size)
        {
                mesh->vertex_buffer_data =
                    realloc(mesh->vertex_buffer_data, mesh->vertex_buffer_size * 2 * sizeof(float));
                mesh->vertex_buffer_size *= 2;
        }

        for (uint i = 0; i < 6; i++)
        {
                uint index = MESH_BLOCK_INDICES[direction][i];

                vec2 texture_coordinates;
                get_real_texture_coords(face_texture_coords, direction, index, texture_coordinates);

                vec3 vertex;
                glm_vec3_add(chunk_block_pos, MESH_BLOCK_VERTICES[index], vertex);

                mesh->vertex_buffer_data[mesh->vertex_buffer_count++] = vertex[0];
                mesh->vertex_buffer_data[mesh->vertex_buffer_count++] = vertex[1];
                mesh->vertex_buffer_data[mesh->vertex_buffer_count++] = vertex[2];
                mesh->vertex_buffer_data[mesh->vertex_buffer_count++] = texture_coordinates[0];
                mesh->vertex_buffer_data[mesh->vertex_buffer_count++] = texture_coordinates[1];
        }
}

void
mesh_prepare_render(struct Mesh* mesh)
{
        vao_bind(&mesh->vao);
        vbo_bind(&mesh->vbo);
        vbo_buffer(&mesh->vbo, mesh->vertex_buffer_data, mesh->vertex_buffer_count * sizeof(float));
        vao_attr(&mesh->vao, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        vao_attr(&mesh->vao, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
}

void
mesh_render(struct Mesh* mesh)
{
        vao_bind(&mesh->vao);
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_buffer_count);
        glBindVertexArray(0);
}

internal void
mesh_check_buffer_size(struct Mesh* mesh, uint amount)
{
        if (mesh->vertex_buffer_count + amount >= mesh->vertex_buffer_size)
                mesh->vertex_buffer_data =
                    realloc(mesh->vertex_buffer_data, mesh->vertex_buffer_size * 2 * sizeof(float));
        mesh->vertex_buffer_size = mesh->vertex_buffer_size * 2;
}

// NOTE(fonsi): el cuadro de texturas es 32 x 15 bloques, tener esto en cuenta para cambiar 'scale'
internal void
get_real_texture_coords(vec2 face_text_position, enum Direction direction, uint vertex_index, vec2 dest)
{
        float scaleX = 1.0f / 16.0f;
        float scaleZ = 1.0f / 16.0f;

        vec2 tex_coords[4];
        for (uint i = 0; i < 4; i++)
                glm_vec2_copy(face_text_position, tex_coords[i]);

        // 0, 0 (0)
        tex_coords[0][0] *= scaleX;
        tex_coords[0][1] *= scaleZ;
        // 1, 0 (1)
        tex_coords[1][0] += 1.0f;
        tex_coords[1][0] *= scaleX;
        tex_coords[1][1] *= scaleZ;
        // 0, 1 (2)
        tex_coords[2][0] += 1.0f;
        tex_coords[2][1] += 1.0f;
        tex_coords[2][0] *= scaleX;
        tex_coords[2][1] *= scaleZ;
        // 1, 1 (3)
        tex_coords[3][1] += 1.0f;
        tex_coords[3][0] *= scaleX;
        tex_coords[3][1] *= scaleZ;

        switch (vertex_index)
        {
        case 0:
                if (direction == UP)
                        glm_vec2_copy(tex_coords[3], dest);
                if (direction == NORTH)
                        glm_vec2_copy(tex_coords[0], dest);
                if (direction == WEST)
                        glm_vec2_copy(tex_coords[1], dest);
                break;
        case 1:
                if (direction == UP)
                        glm_vec2_copy(tex_coords[2], dest);
                if (direction == NORTH)
                        glm_vec2_copy(tex_coords[1], dest);
                if (direction == EAST)
                        glm_vec2_copy(tex_coords[0], dest);
                break;
        case 2:
                if (direction == DOWN)
                        glm_vec2_copy(tex_coords[3], dest);
                if (direction == NORTH)
                        glm_vec2_copy(tex_coords[2], dest);
                if (direction == EAST)
                        glm_vec2_copy(tex_coords[3], dest);
                break;
        case 3:
                if (direction == DOWN)
                        glm_vec2_copy(tex_coords[2], dest);
                if (direction == NORTH)
                        glm_vec2_copy(tex_coords[3], dest);
                if (direction == WEST)
                        glm_vec2_copy(tex_coords[2], dest);
                break;
        case 4:
                if (direction == UP)
                        glm_vec2_copy(tex_coords[0], dest);
                if (direction == SOUTH)
                        glm_vec2_copy(tex_coords[1], dest);
                if (direction == WEST)
                        glm_vec2_copy(tex_coords[0], dest);
                break;
        case 5:
                if (direction == UP)
                        glm_vec2_copy(tex_coords[1], dest);
                if (direction == SOUTH)
                        glm_vec2_copy(tex_coords[0], dest);
                if (direction == EAST)
                        glm_vec2_copy(tex_coords[1], dest);
                break;
        case 6:
                if (direction == DOWN)
                        glm_vec2_copy(tex_coords[0], dest);
                if (direction == SOUTH)
                        glm_vec2_copy(tex_coords[3], dest);
                if (direction == EAST)
                        glm_vec2_copy(tex_coords[2], dest);
                break;
        case 7:
                if (direction == DOWN)
                        glm_vec2_copy(tex_coords[1], dest);
                if (direction == SOUTH)
                        glm_vec2_copy(tex_coords[2], dest);
                if (direction == WEST)
                        glm_vec2_copy(tex_coords[3], dest);
                break;
        };
}