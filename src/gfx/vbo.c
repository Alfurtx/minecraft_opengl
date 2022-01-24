#include "gfx.h"
#include "vbo.h"

struct VBO
vbo_create(uint buffer_type, bool dynamic)
{
        struct VBO res;

        res.buffer_type = buffer_type;
        res.dynamic     = dynamic;
        glGenBuffers(1, &res.handle);

        return (res);
}

void
vbo_destroy(struct VBO* vbo)
{
        glDeleteBuffers(1, &vbo->handle);
}

void
vbo_buffer(struct VBO* vbo, const void* data, size_t size)
{
        glBufferData(vbo->buffer_type, size, data, vbo->dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
}

void
vbo_bind(struct VBO* vbo)
{
        glBindBuffer(vbo->buffer_type, vbo->handle);
}
