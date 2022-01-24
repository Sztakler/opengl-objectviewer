#include "mesh.h"

Mesh::Mesh(const char *obj_data_filename, const char *vertex_shader_filename, const char *fragment_shader_filename, std::vector<Texture> &textures)
{

    // loadFromObjectFile(obj_data_filename);
    objLoader(obj_data_filename);

    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    this->vao = VAO();
    this->vao.Bind();
    this->vbo = VBO(this->vertices, this->vertices.size() * sizeof(vertex_t));

    this->ebo = EBO(this->indices, this->indices.size() * sizeof(GLuint));

    this->vao.link_attribute(vbo, 0, 3, GL_FLOAT, sizeof(vertex_t), (void *)0);                   // position
    this->vao.link_attribute(vbo, 1, 3, GL_FLOAT, sizeof(vertex_t), (void *)(3 * sizeof(float))); // normal
    this->vao.link_attribute(vbo, 2, 2, GL_FLOAT, sizeof(vertex_t), (void *)(6 * sizeof(float))); // textureUV

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);

    this->vao.Unbind();
    this->vbo.Unbind();
    this->ebo.Unbind();
}

void Mesh::Draw(ArcballCamera &camera, glm::mat4 &MVP_matrix, glm::mat4 &model, DrawableLight light)
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

        textures[i].tex_unit(shader, (type + num).c_str(), i);
        textures[i].Bind();
    }

    Material material = {
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        16.0f};

    /* Send camera position to shader. */
    glUniform3fv(glGetUniformLocation(shader.id, "cameraPosition"), 1, glm::value_ptr(camera.pos));

    /* Send MVP matrix to shader. */
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "mvp"), 1, GL_FALSE, glm::value_ptr(MVP_matrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.position"), 1, glm::value_ptr(light.position));
    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.ambient"), 1, glm::value_ptr(light.ambient));
    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.diffuse"), 1, glm::value_ptr(light.diffuse));
    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.specular"), 1, glm::value_ptr(light.specular));
    glUniform1f(glGetUniformLocation(shader.id, "pointlight.constant"), light.constant);
    glUniform1f(glGetUniformLocation(shader.id, "pointlight.linear"), light.linear);
    glUniform1f(glGetUniformLocation(shader.id, "pointlight.quadratic"), light.quadratic);

    glUniform3fv(glGetUniformLocation(shader.id, "material.ambient"), 1, glm::value_ptr(material.ambient));
    glUniform3fv(glGetUniformLocation(shader.id, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(shader.id, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f(glGetUniformLocation(shader.id, "material.shininess"), material.shininess);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    for (uint32_t i = 0; i < textures.size(); i++)
    {
        textures[i].Unbind();
    }
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::Draw(ArcballCamera &camera, glm::mat4 &MVP_matrix, glm::mat4 &model, DrawableLight light, float delta_time)
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

        textures[i].tex_unit(shader, (type + num).c_str(), i);
        textures[i].Bind();
    }

    Material material = {
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        16.0f};

    /* Send time data to shader. */
    glUniform1f(glGetUniformLocation(shader.id, "delta_time"), delta_time);

    /* Send camera position to shader. */
    glUniform3fv(glGetUniformLocation(shader.id, "cameraPosition"), 1, glm::value_ptr(camera.pos));

    /* Send MVP matrix to shader. */
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "mvp"), 1, GL_FALSE, glm::value_ptr(MVP_matrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.position"), 1, glm::value_ptr(light.position));
    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.ambient"), 1, glm::value_ptr(light.ambient));
    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.diffuse"), 1, glm::value_ptr(light.diffuse));
    glUniform3fv(glGetUniformLocation(shader.id, "pointlight.specular"), 1, glm::value_ptr(light.specular));
    glUniform1f(glGetUniformLocation(shader.id, "pointlight.constant"), light.constant);
    glUniform1f(glGetUniformLocation(shader.id, "pointlight.linear"), light.linear);
    glUniform1f(glGetUniformLocation(shader.id, "pointlight.quadratic"), light.quadratic);

    glUniform3fv(glGetUniformLocation(shader.id, "material.ambient"), 1, glm::value_ptr(material.ambient));
    glUniform3fv(glGetUniformLocation(shader.id, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(shader.id, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f(glGetUniformLocation(shader.id, "material.shininess"), material.shininess);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    for (uint32_t i = 0; i < textures.size(); i++)
    {
        textures[i].Unbind();
    }
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

bool Mesh::loadFromObjectFile(const char *filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        return false;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texUV;

    glm::vec3 vn = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 v = glm::vec3(0.0, 0.0, 0.0);
    glm::vec2 vt = glm::vec2(0.0, 0.0);

    while (!f.eof())
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

                char *token = strtok(line, delimiter);
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
            else if (line[1] == 't')
            {
                char delimiter[] = " ";
                std::vector<std::string> tokens;

                char *token = strtok(line, delimiter);
                while (token)
                {
                    tokens.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                vt.x = stof(tokens[1]);
                vt.y = stof(tokens[2]);

                temp_texUV.push_back(vt);
            }
            else
            {
                char delimiter[] = " ";
                std::vector<std::string> tokens;

                char *token = strtok(line, delimiter);
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
            int t[3];

            std::string parts[3];

            s >> junk >> parts[0] >> parts[1] >> parts[2];

            // std::cout << junk << " " << parts[0] << " " << parts[1] << " " << parts[2] << "\n";

            for (int i = 0; i < 3; i++)
            {
                // replace(parts[i], "//", " ");

                std::cout << line << "\n";
                char delimiter[] = "/";
                std::vector<std::string> numbers;

                char *token = strtok(const_cast<char *>(parts[i].c_str()), delimiter);
                while (token != nullptr)
                {
                    numbers.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }
                // std::cout << numbers.size() << "\n";
                // std::cout << junk << " " << numbers[0] << " " << numbers[1] << " " << numbers[2] << "\n";
                f[i] = stoi(numbers[0]);
                t[i] = stoi(numbers[1]);
                n[i] = stoi(numbers[2]);
                // std::cout << junk << " " << f[i] << " " << t[i] << " " << n[i] << "\n";
            }

            for (int i = 0; i < 3; i++)
            {
                vertex_t vertex = {.position = temp_vertices[f[i] - 1],
                                   .normal = temp_normals[n[i] - 1],
                                   .textureUV = temp_texUV[t[i] - 1]};
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

    print_vertices();
}

bool Mesh::objLoader(const char *filename)
{
    std::vector<uint> vertex_indexes, uv_indexes, normal_indexes;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("\033[91mCouldn't open file %s.\n\033[0m", filename);
        return false;
    }

    while (true)
    {
        char line_header[128];

        int res = fscanf(file, "%s", line_header);
        if (res == EOF)
            break;

        // printf("line_header = %s\n", line_header);

        if (strcmp(line_header, "v") == 0) // vertex position
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(line_header, "vt") == 0) // uv coordinates
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(line_header, "vn") == 0) // normal vector
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(line_header, "f") == 0) // face
        {
            std::string vertex1, vertex2, vertex3;
            uint vertex_index[3], uv_index[3], normal_index[3];

            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertex_index[0], &uv_index[0], &normal_index[0],
                                 &vertex_index[1], &uv_index[1], &normal_index[1],
                                 &vertex_index[2], &uv_index[2], &normal_index[2]);

            if (matches != 9)
            {
                printf("\033[91mFile %s couldn't be parsed.\nTry exporting with other options (needs vertex, textureUV and normals data).\n\
Check, whether faces are triangulated,\033[0m", filename);
                return false;
            }
            vertex_indexes.push_back(vertex_index[0]);
            vertex_indexes.push_back(vertex_index[1]);
            vertex_indexes.push_back(vertex_index[2]);
            uv_indexes    .push_back(uv_index[0]);
            uv_indexes    .push_back(uv_index[1]);
            uv_indexes    .push_back(uv_index[2]);
            normal_indexes.push_back(normal_index[0]);
            normal_indexes.push_back(normal_index[1]);
            normal_indexes.push_back(normal_index[2]);
        }
    }

    // printf("vertex_indexes.size() = %d\n", vertex_indexes.size());

    for (uint i = 0; i < vertex_indexes.size(); i++)
    {
        uint vertex_index = vertex_indexes[i];
        glm::vec3 vertex = temp_vertices[vertex_index - 1];

        uint uv_index = uv_indexes[i];
        glm::vec2 uv = temp_uvs[uv_index - 1];

        uint normal_index = normal_indexes[i];
        glm::vec3 normal = temp_normals[normal_index - 1];

        vertex_t v = {vertex, normal, uv};

        // printf("v: %f %f %f n: %f %f %f t:%f %f\n",
            // v.position.x, v.position.y, v.position.z,
            // v.normal.x, v.normal.y, v.normal.z,
            // v.textureUV.x, v.textureUV.y);

        this->vertices.push_back(v);
    }

    fclose(file);
    return true;
}

void Mesh::print_vertices()
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

void Mesh::print_indices()
{
    for (int i = 0; i < this->indices.size(); i++)
    {
        printf("[%d] index: %d\n", i, indices[i]);
    }
}

bool Mesh::replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}