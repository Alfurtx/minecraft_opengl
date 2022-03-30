#ifndef VAO_H_
#define VAO_H_

#include "../utils/types.h"

struct VAO {
        uint handle;
};

extern struct VAO vao_create();
extern void       vao_destroy(struct VAO* vao);
extern void       vao_bind(struct VAO* vao);
extern void       vao_attr(struct VAO* vao, uint index, uint size, uint type, bool normalized,
                           uint stride, const void* offset);

#endif // VAO_H_
