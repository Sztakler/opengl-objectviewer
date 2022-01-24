#ifndef TEXTURE_H
#define TEXTURE_H

#include "includes.h"
#include "shader.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

class Texture
{
    public:
        GLuint id;
        const char* type;
        GLuint unit;

    public:
        Texture(const char* image, const char* tex_type, GLenum slot, GLenum format, GLenum pixel_type);
        Texture(const char* image, GLenum tex_type, GLuint slot, GLenum format, GLenum pixel_type);

    public:
        /* Assign a texture unit to a texture. */
        void tex_unit(Shader& shader, const char* uniform, GLuint unit);

        /* Bind a texture. */
        void Bind();

        /* Unbind a texture. */
        void Unbind();

        /* Deletes texture. */
        void Delete();
};

#endif // !TEXTURE_H