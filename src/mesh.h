#ifndef MESH_H
#define MESH_H

#include "includes.h"

#include "vao.h"
#include "vbo.h"
#include "ebo.h"

#include "arcball_camera.h"
#include "drawable_light.h"
#include "texture.h"
#include "shader.h"


class Mesh
{
public:
    VAO vao;
    VBO vbo;
    EBO ebo;

    std::vector<vertex_t> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    Shader shader;

public:
    Mesh(const char* obj_data_filename, const char* vertex_shader_filename, const char* fragment_shader_filename, std::vector<Texture> &textures);

public:
    void Draw(ArcballCamera &camera, glm::mat4 &MVP_matrix, glm::mat4 &model, DrawableLight light);
    void print_vertices();
    void print_indices();

private:
    bool replace(std::string &str, const std::string &from, const std::string &to);
    bool loadFromObjectFile(const char *filename);
    bool objLoader(const char* filename);
};

#endif