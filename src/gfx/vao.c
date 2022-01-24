#include "gfx.h"
#include "vao.h"
#include <stdbool.h>

struct VAO
vao_create()
{
        struct VAO res;

        glGenVertexArrays(1, &res.handle);

        return (res);
}

void
vao_destroy(struct VAO* vao)
{
        glDeleteVertexArrays(1, &vao->handle);
}

void
vao_bind(struct VAO* vao)
{
        glBindVertexArray(vao->handle);
}

void
vao_attr(struct VAO* vao, uint index, uint size, uint type, bool normalized, uint stride, const void* offset)
{
        vao_bind(vao);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalized, stride, offset);
}
