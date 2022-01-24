#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

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

struct Material { 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcPointLight(PointLight light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform vec3 cameraPosition;
uniform PointLight pointlight;
uniform Material material;

void main()
{
	vec3 norm = normalize(normal);
	vec3 viewDirection = normalize(cameraPosition - pos);
	vec3 lightDirection = normalize(pointlight.position - pos);

	vec3 lightDir = normalize(pointlight.position - pos);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);

	// Calculate specular component only if pixel is facing the light (remove specular light for rear faces of an object)
	float spec = 0;
	if (diff > 0.0)
		spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    // // attenuation
    float distance = length(pointlight.position - pos);
    float attenuation = 1.0 / (pointlight.constant + pointlight.linear * distance + pointlight.quadratic * (distance * distance));
    // combine results
    vec3 ambient = pointlight.ambient * material.diffuse;
    vec3 diffuse = pointlight.diffuse * diff * material.diffuse;
    vec3 specular = pointlight.specular * spec * material.specular;

    diffuse *= attenuation;
    specular *= attenuation;

	vec3 result = ambient + diffuse + specular;

	FragColor = texture(diffuse0, texUV) * vec4(diffuse + ambient, 1.0) + texture(specular0, texUV).r * vec4(specular, 1.0);

}


vec3 CalcPointLight(PointLight light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}