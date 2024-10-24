#version 330 core
out vec4 FragColor;

in vec3 TexCoord; // Coordonnées de texture reçues du vertex shader

uniform samplerCube skybox; // Texture

void main()
{
    FragColor = texture(skybox, TexCoord); // Échantillonne la texture
}

