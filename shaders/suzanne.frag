#version 330 core
out vec4 FragColor;

in vec3 position;

void main()
{
    vec3 pos = position + 0.5;
    FragColor = vec4(pos.x, pos.y, pos.z, 1.0);
}