#version 330 core
layout(location = 0) in vec3 aPos; // Position des sommets

out vec3 TexCoords; // Coordonnées de texture 3D

uniform mat4 view; // Matrice de vue
uniform mat4 projection; // Matrice de projection

void main()
{
    TexCoords = aPos; // Passez les coordonnées de position comme coordonnées de texture
    gl_Position = projection * view * vec4(aPos, 1.0); // Calcul de la position
}

