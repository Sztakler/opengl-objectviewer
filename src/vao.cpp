#include "vao.h"

        VAO::VAO()
        {
            glGenVertexArrays(1, &id);
        }

        void VAO::link_vbo(VBO vbo, GLuint layout, GLuint size, int type)
        {
            vbo.Bind();
            glVertexAttribPointer(layout, size, type, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(layout);
            vbo.Unbind();
        }

        void VAO::link_instance_vbo(VBO vbo, GLuint layout, GLuint size, GLuint divisor, int type)
        {
            vbo.Bind();
            glVertexAttribPointer(layout, size, type, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(layout);
            vbo.Unbind();
            glVertexAttribDivisor(layout, divisor);
        }

        void VAO::Bind()
        {
            glBindVertexArray(id);
        }

        void VAO::Unbind()
        {
            glBindVertexArray(0);
        }

        void VAO::Delete()
        {
            glDeleteVertexArrays(1, &id);
        }