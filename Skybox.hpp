// Skybox.h
#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();

    void draw( const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint loadCubemap(const std::vector<std::string>& faces);
    GLuint skyboxVAO, skyboxVBO;
    GLuint cubemapTexture;
    GLuint shaderProgramSkybox;

    void setupSkybox();
};

#endif
