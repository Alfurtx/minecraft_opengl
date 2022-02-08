#include "../utils/utils.h"
#include "gfx.h"
#include "texture.h"

struct Texture
texture_create(const char* texture_file_path)
{
        struct Texture res = {0};

        res.texture_type = GL_TEXTURE_2D;
        glGenTextures(1, &res.handle);

        int            width, height, num_components;
        unsigned char* data = stbi_load(texture_file_path, &width, &height, &num_components, 0);
        if (data)
        {
                GLenum format;
                switch (num_components)
                {
                case 1: format = GL_RED; break;
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
                }

                glBindTexture(res.texture_type, res.handle);
                glTexImage2D(res.texture_type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(res.texture_type);

                glTexParameteri(res.texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(res.texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(res.texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTexParameteri(res.texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                stbi_image_free(data);
        } else {
                fprintf(stderr, "Failed to load texture path\n");
                stbi_image_free(data);
        }

        return (res);
}

void
texture_destroy(struct Texture* texture)
{
        glDeleteTextures(1, &texture->handle);
}

void
texture_bind(struct Texture* texture)
{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(texture->texture_type, texture->handle);
}
