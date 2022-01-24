#include "drawable_light.h"

DrawableLight::DrawableLight( const char* obj_data_filename,
               const char* vertex_shader_filename,
               const char* fragment_shader_filename,
               glm::vec3 position,
               glm::vec3 ambient,
               glm::vec3 diffuse,
               glm::vec3 specular,
               float constant, float linear, float quadratic)
{

    loadFromObjectFile(obj_data_filename);

    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    this->position = position;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;

    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;


    this->vao = VAO();
    this->vao.Bind();
    this->vbo = VBO(this->vertices, this->vertices.size() * sizeof(vertex_t));

    this->ebo = EBO(this->indices, this->indices.size() * sizeof(GLuint));

    this->vao.link_attribute(vbo, 0, 3, GL_FLOAT, sizeof(vertex_t), (void*)0);                    // position
    this->vao.link_attribute(vbo, 1, 3, GL_FLOAT, sizeof(vertex_t), (void*)(3 * sizeof(float)));  // normal
    this->vao.link_attribute(vbo, 2, 2, GL_FLOAT, sizeof(vertex_t), (void*)(6 * sizeof(float)));  // textureUV

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);

    this->vao.Unbind();
    this->vbo.Unbind();
    this->ebo.Unbind();
}


void DrawableLight::Draw(ArcballCamera &camera, glm::mat4 &MVP_matrix)
{
    shader.Activate();
    vao.Bind();

    uint32_t n_diffuse = 0;
    uint32_t n_specular = 0;

    for (uint32_t i = 0; i < textures.size(); i++)
    {
        std::string num;
        std::string type = textures[i].type;

        if (type == "diffuse")
        {
            num = std::to_string(n_diffuse++);
        }
        else if (type == "specular")
        {
            num = std::to_string(n_specular++);
        }
        std::cout << (type+num).c_str() << "\n";
        textures[i].tex_unit(shader, (type + num).c_str(), i);
        textures[i].Bind();
    }


    /* Send camera position to shader. */
    // glUniform3f(glGetUniformLocation(shader.id, "camera_position"), camera.position.x, camera.position.y, camera.position.z);

    /* Send MVP matrix to shader. */
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "mvp"), 1, GL_FALSE, glm::value_ptr(MVP_matrix));


    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

bool DrawableLight::loadFromObjectFile(const char* filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        return false;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    glm::vec3 vn = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 v = glm::vec3(0.0, 0.0, 0.0);

    while(!f.eof())
    {
        char line[128];
        f.getline(line, 128);

        std::stringstream s;
        s << line;


        char junk;

        if (line[0] == 'v')
        {
            if (line[1] == 'n')
            {
                char delimiter[] = " ";
                std::vector<std::string> tokens;

                char* token = strtok(line, delimiter);
                while (token)
                {
                    tokens.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                vn.x = stof(tokens[1]);
                vn.y = stof(tokens[2]);
                vn.z = stof(tokens[3]);

                temp_normals.push_back(vn);
            }
            else
            {
                char delimiter[] = " ";
                std::vector<std::string> tokens;

                char* token = strtok(line, delimiter);
                while (token)
                {
                    tokens.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                v.x = stof(tokens[1]);
                v.y = stof(tokens[2]);
                v.z = stof(tokens[3]);

                temp_vertices.push_back(v);
            }
        }

        if (line[0] == 'f')
        {
            int f[3];
            int n[3];

            std::string parts[3];

            s >> junk >> parts[0] >> parts[1] >> parts[2];

            for (int i = 0; i < 3; i++)
            {
                replace(parts[i], "//", " ");

                char delimiter[] = " ";
                std::vector<std::string> numbers;

                char* token = strtok(const_cast<char*>(parts[i].c_str()), delimiter);
                while (token != nullptr)
                {
                    numbers.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                f[i] = stoi(numbers[0]);
                n[i] = stoi(numbers[1]);
            }

            for (int i = 0; i < 3; i++)
            {
                vertex_t vertex = {.position  = temp_vertices[f[i] - 1],
                                   .normal    = temp_normals[n[i] - 1],
                                   .textureUV = (i == 0) ? glm::vec2(0.0, 0.0)
                                                        : (i == 1) ? glm::vec2(0.0, 1.0)
                                                                   : glm::vec2(1.0, 1.0)};
                this->vertices.push_back(vertex);
                this->indices.push_back(f[i] - 1);
                // this->vertices.push_back(temp_vertices[f[i] - 1].x);
                // this->vertices.push_back(temp_vertices[f[i] - 1].y);
                // this->vertices.push_back(temp_vertices[f[i] - 1].z);

                // this->normals.push_back(temp_normals[n[i] - 1].x);
                // this->normals.push_back(temp_normals[n[i] - 1].y);
                // this->normals.push_back(temp_normals[n[i] - 1].z);
            }

        }
    }
    return true;
}

void DrawableLight::print_vertices()
{
    for (int i = 0; i < this->vertices.size(); i++)
    {
        printf("[%d] position: (%f %f %f), normal: (%f %f %f), texture_uv: (%f %f)\n",
            i,
            vertices[i].position.x, vertices[i].position.y, vertices[i].position.z,
            vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z,
            vertices[i].textureUV.x, vertices[i].textureUV.y);
    }
}

void DrawableLight::print_indices()
{
    for (int i = 0; i < this->indices.size(); i++)
    {
        printf("[%d] index: %d\n", i, indices[i]);
    }
}

bool DrawableLight::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}