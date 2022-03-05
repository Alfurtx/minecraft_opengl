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

struct Vertex
{
        vec3 pos;
        vec2 texture_pos;
};

struct Mesh
{
        struct VAO vao;
        struct VBO vbo;

        struct Vertex* vertex_arr;
};

extern void mesh_init(struct Mesh* mesh);
extern void mesh_destroy(struct Mesh* mesh);
extern void mesh_add_face(struct Mesh* mesh, vec3 chunk_block_pos, vec2 face_texture_coords, enum Direction direction);
extern void mesh_prepare_render(struct Mesh* mesh);
extern void mesh_render(struct Mesh* mesh);

// The quad positions are:
// 0 -> top left
// 1 -> top right
// 2 -> bottom right
// 3 -> bottom left
extern void mesh_add_quad(struct Mesh*   mesh,
                          vec3           quad_positions[4],
                          vec2           texture_position,
                          enum BlockType block_type,
                          enum Direction face_direction);

/*

 QUAD DESCRIPTION

  p1 -------- p2
  |            |
  |            |
  |            |
  |            |
  p4 -------- p3

*/

#endif // MESH_H_
