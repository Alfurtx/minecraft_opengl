#ifndef SHADER_H_
#define SHADER_H_

#include "../utils/types.h"
#include "gfx.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdbool.h>

struct Shader
{
        uint handle;
};

extern struct Shader shader_create(const char* vertex_file_path, const char* fragment_file_path);
extern void          shader_use(struct Shader* shader);
extern void          shader_set_uniform_vec2(struct Shader* shader, const char* name, size_t count, vec2 value);
extern void          shader_set_uniform_vec3(struct Shader* shader, const char* name, size_t count, vec3 value);
extern void          shader_set_uniform_mat4(struct Shader* shader, const char* name, size_t count, bool transpose, const float* value);
extern void          shader_set_uniform_int(struct Shader* shader, const char* name, size_t count, int value);
extern void          shader_set_uniform_uint(struct Shader* shader, const char* name, size_t count, uint value);

#endif // SHADER_H_
