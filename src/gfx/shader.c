#include "../utils/types.h"
#include "gfx.h"
#include "shader.h"
#include <cglm/cglm.h>
#include <stdio.h>

struct Shader
shader_create(const char* vertex_file_path, const char* fragment_file_path)
{
        struct Shader res;

        FILE* fp;

        uint file_size;

        char* vertex_src   = NULL;
        char* fragment_src = NULL;

        int  success;
        char log[512];

        uint vertex_id   = glCreateShader(GL_VERTEX_SHADER);
        uint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

        fp = fopen(vertex_file_path, "rb");
        if (fp)
        {
                fseek(fp, 0L, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                vertex_src = calloc(1, file_size + 1);
                if (vertex_src)
                        fread(vertex_src, 1, file_size, fp);
                fclose(fp);
        }

        fp = fopen(fragment_file_path, "rb");
        if (fp)
        {
                fseek(fp, 0L, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                fragment_src = calloc(1, file_size + 1);
                if (fragment_src)
                        fread(fragment_src, 1, file_size, fp);
                fclose(fp);
        }

        const char* vsrc = vertex_src;
        const char* fsrc = fragment_src;

        glShaderSource(vertex_id, 1, &vsrc, NULL);
        glCompileShader(vertex_id);
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(vertex_id, 512, NULL, log);
        if (!success)
                fprintf(stderr, "Failed compiling vertex shader\nDescription: %s\n", log);

        glShaderSource(fragment_id, 1, &fsrc, NULL);
        glCompileShader(fragment_id);
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(fragment_id, 512, NULL, log);
        if (!success)
                fprintf(stderr, "Failed compiling fragment shader\nDescription: %s\n", log);

        res.handle = glCreateProgram();
        glAttachShader(res.handle, vertex_id);
        glAttachShader(res.handle, fragment_id);
        glLinkProgram(res.handle);

        glGetProgramiv(res.handle, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(res.handle, 512, NULL, log);
        if (!success)
                fprintf(stderr, "Failed linking shader\nDescription: %s\n", log);

        glDetachShader(res.handle, vertex_id);
        glDetachShader(res.handle, fragment_id);
        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
        if (vertex_src && fragment_src)
        {
                free(vertex_src);
                free(fragment_src);
        }

        return (res);
}

void
shader_use(struct Shader* shader)
{
        glUseProgram(shader->handle);
}

void
shader_set_uniform_vec2(struct Shader* shader, const char* name, size_t count, vec2 value)
{
        glUniform2fv(glGetUniformLocation(shader->handle, name), count, value);
}

void
shader_set_uniform_vec3(struct Shader* shader, const char* name, size_t count, vec3 value)
{
        glUniform3fv(glGetUniformLocation(shader->handle, name), count, value);
}

void
shader_set_uniform_mat4(struct Shader* shader, const char* name, size_t count, bool transpose, const float* value)
{
        glUniformMatrix4fv(glGetUniformLocation(shader->handle, name), count, transpose, value);
}

void
shader_set_uniform_int(struct Shader* shader, const char* name, size_t count, int value)
{
        glUniform1i(glGetUniformLocation(shader->handle, name), value);
}

void
shader_set_uniform_uint(struct Shader* shader, const char* name, size_t count, uint value)
{
        glUniform1ui(glGetUniformLocation(shader->handle, name), value);
}
