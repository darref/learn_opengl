#version 330 core
out vec4 FragColor;

in vec2 TexCoord; // Coordonnées de texture reçues du vertex shader

uniform sampler2D texture1; // Texture

void main()
{
    FragColor = texture(texture1, TexCoord); // Échantillonne la texture
}

