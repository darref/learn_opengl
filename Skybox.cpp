#include "Skybox.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Shaders (déclarés sous forme de chaînes de caractères)
const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
in vec3 FragPos;

uniform vec3 sunDirection;

void main() {
    // Simulate Rayleigh scattering (very simplified)
    float cosAngle = dot(normalize(FragPos), normalize(sunDirection));
    float rayleighScatter = max(0.0, cosAngle);

    // Set color based on scattering effect (blue sky)
    vec3 skyColor = mix(vec3(0.3, 0.4, 1.0), vec3(1.0, 0.5, 0.0), rayleighScatter);
    FragColor = vec4(skyColor, 1.0);
}
)";

Skybox::Skybox(float planetRadius, float atmosphereRadius)
    : planetRadius(planetRadius), atmosphereRadius(atmosphereRadius), VAO(0), VBO(0), shaderProgram(0) {}

void Skybox::init() {
    // Générer la géométrie (une sphère qui représente l'atmosphère)
    createSphere(atmosphereRadius, 64, 32);

    // Initialisation des buffers OpenGL
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Charger les shaders
    loadShaders();

    // Obtenir les locations des uniformes
    atmosphereColorLoc = glGetUniformLocation(shaderProgram, "atmosphereColor");
    viewMatrixLoc = glGetUniformLocation(shaderProgram, "view");
    projectionMatrixLoc = glGetUniformLocation(shaderProgram, "projection");
    sunDirectionLoc = glGetUniformLocation(shaderProgram, "sunDirection");
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& sunDirection) {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(sunDirectionLoc, 1, glm::value_ptr(sunDirection));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}

void Skybox::loadShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Skybox::createSphere(float radius, int sectors, int stacks) {
    const float PI = 3.14159265359f;

    float x, y, z, xy;                              // positions
    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i) {
        stackAngle = PI / 2 - i * stackStep;        // de pi/2 à -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;           // de 0 à 2pi

            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Ajouter des indices pour créer les triangles ici (optionnel).
    // Dans cet exemple, on utilise directement les vertices pour GL_TRIANGLES.
}

