#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

out vec3 pos;
out vec3 norm;
out vec2 texuv;

void main()
{
    pos = aPosition;
    norm = aNormal;
    texuv = aTexture;

    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}