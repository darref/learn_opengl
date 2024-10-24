#ifndef Skybox_HPP
#define Skybox_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <string>

class Skybox {
public:
    Skybox(float planetRadius, float atmosphereRadius);
    void init();
    void draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& sunDirection);

private:
    GLuint VAO, VBO;
    GLuint shaderProgram;
    GLuint atmosphereColorLoc, viewMatrixLoc, projectionMatrixLoc, sunDirectionLoc;

    float planetRadius;
    float atmosphereRadius;

    void loadShaders();
    void createSphere(float radius, int sectors, int stacks);
    
    std::vector<float> vertices;  // Stockage des vertices pour la sphère
};

#endif

