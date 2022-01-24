#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aNormal;
// // Texture Coordinates
layout (location = 2) in vec2 aTex;

uniform mat4 mvp;
// uniform vec3 camera_position;

out vec3 pos;
out vec3 normal;
out vec2 texUV;


void main()
{
	pos = aPos;
	// Assigns the normals from the Vertex Data to "normal"
	normal = aNormal;
	// Assigns the texture coordinates from the Vertex Data to "texUV"
	texUV = aTex;

	// Outputs the positions/coordinates of all vertices
	gl_Position = mvp * vec4(aPos, 1.0f);
}