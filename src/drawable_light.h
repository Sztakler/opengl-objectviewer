#ifndef DRAWABLE_LIGHT_H
#define DRAWABLE_LIGHT_H

#include "includes.h"

#include "vao.h"
#include "vbo.h"
#include "ebo.h"

#include "arcball_camera.h"
#include "texture.h"
#include "shader.h"


class DrawableLight
{
    public:
        VAO vao;
        VBO vbo;
        EBO ebo;

        std::vector<vertex_t> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        Shader shader;

        /* Light properties */
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float speed;

    public:
        DrawableLight(const char* obj_data_filename,
               const char* vertex_shader_filename,
               const char* fragment_shader_filename);
        DrawableLight(const char* obj_data_filename,
               const char* vertex_shader_filename,
               const char* fragment_shader_filename,
               glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
               glm::vec3 ambient = glm::vec3(0.2, 0.2, 0.2),
               glm::vec3 diffuse = glm::vec3(0.5, 0.5, 0.5),
               glm::vec3 specular = glm::vec3(1.0, 1.0, 1.0),
               float constant = 0.5, float linear = 0.009, float quadratic = 0.02);

    public:
        void Draw(ArcballCamera &camera, glm::mat4 &MVP_matrix);
        void Draw(glm::mat4* model, glm::mat4* view, glm::mat4* projection, DRAWING_MODE drawing_mode, bool transparent, glm::vec3 camera_position);
        void loadUniforms(); // TODO
        void print_vertices();
        void print_indices();

    private:
        bool replace(std::string& str, const std::string& from, const std::string& to);
        bool loadFromObjectFile(const char* filename);
};

#endif