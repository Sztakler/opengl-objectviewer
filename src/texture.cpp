#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char* image, const char* tex_type, GLenum slot, GLenum pixel_type)
{
    type = tex_type;
	printf("%s\n", image);
    int tex_width, tex_height, tex_nChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes = stbi_load(image, &tex_width, &tex_height, &tex_nChannels, 0);

    /* Create a texture. */
    glGenTextures(1, &id);
    /* Assign texture to a texture unit. */
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    printf("%d %d %d\n", tex_width, tex_height, tex_nChannels);

    GLenum channels = GL_RGBA;
    switch (tex_nChannels)
    {
        case 1:
            channels = GL_RED;
            printf("GL_RED\n");
            break;
        case 3:
            channels = GL_RGB;
            printf("GL_RGB\n");
            break;
        case 4:
            channels = GL_RGBA;
            printf("GL_RGBA\n");
            break;
        default:
            channels = GL_RGB;
            printf("GL_RGB\n");
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, channels, pixel_type, tex_bytes);


    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(tex_bytes);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const char* image, GLenum tex_type, GLuint slot, GLenum format, GLenum pixel_type)
{

    int tex_width, tex_height, tex_nChannels;

    // stbi_set_flip_vertically_on_load(true);

    unsigned char* tex_bytes = stbi_load(image, &tex_width, &tex_height, &tex_nChannels, 0);

    /* Create a texture. */
    glGenTextures(1, &id);
    /* Assign texture to a texture unit. */
    glActiveTexture(GL_TEXTURE0);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, format, pixel_type, tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(tex_bytes);

    glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::tex_unit(Shader& shader, const char* uniform, GLuint unit)
{
    /* Get location of the uniform. */
    // std::cout << uniform << " " << unit << std::endl;
    GLuint tex_uniform = glGetUniformLocation(shader.id, uniform);
    shader.Activate();
    glUniform1i(tex_uniform, unit);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &id);
}