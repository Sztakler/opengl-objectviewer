#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


in vec3 pos;
// // Inputs the color from the Vertex Shader
in vec3 normal;
// // Inputs the texture coordinates from the Vertex Shader
in vec2 texUV;

// Gets the Texture Unit from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;


void main()
{
	// FragColor =  texture(specular0, texUV);
    FragColor = vec4(1.0);
}

