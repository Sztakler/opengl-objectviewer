#version 330 core
out vec4 FragColor;

in vec3 pos;
in vec3 norm;
in vec2 texuv;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

void main()
{
    vec3 position = pos;
    vec3 normal = norm;
    vec2 texture = texuv;
    FragColor = vec4(pos.x, pos.y, pos.z, 1.0);
}