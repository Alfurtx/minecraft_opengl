#ifndef VBO_H_
#define VBO_H_

#include "../utils/types.h"
#include <stdbool.h>
#include <stddef.h>

struct VBO
{
        uint handle;
        uint buffer_type;
        bool dynamic;
};

extern struct VBO vbo_create(uint buffer_type, bool dynamic);
extern void       vbo_destroy(struct VBO* vbo);
extern void       vbo_buffer(struct VBO* vbo, const void* data, size_t size);
extern void       vbo_bind(struct VBO* vbo);

#endif // VBO_H_
