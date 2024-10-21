#version 330 core
layout(location = 0) in vec3 aPos; // Position
layout(location = 1) in vec2 aTexCoord; // Coordonnées de texture

out vec2 TexCoord; // Sortie vers le fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord; // Passer les coordonnées de texture au fragment shader
}

