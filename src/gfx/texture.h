#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../utils/types.h"

struct Texture
{
        uint handle;
        uint texture_type;
};

extern struct Texture texture_create(const char* texture_file_path);
extern void texture_destroy(struct Texture* texture);
extern void texture_bind(struct Texture* texture);

#endif // TEXTURE_H_
