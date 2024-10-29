#ifndef MORTAR_HPP
#define MORTAR_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "FPSCounter.hpp"
#include "Camera.hpp"
#include "Shaders.h"
#include "HeightmapTerrain.hpp"
#include "Model.hpp"
#include "Frustum.hpp"
#include "Character.hpp"
#include "Mode.h"
#include "Skybox.hpp"

class Character;

// Déclaration de la classe Mortar
class Mortar
{
private:
    // Attributs privés
    FPSCounter* fps;
    GLFWwindow* window;
    GLuint shaderProgram;
    GLuint shaderProgramSkybox;
    HeightmapTerrain* terrain;
    std::vector<Model*> models;
    Frustum* frustum;
    Character* character;
    Mode currentMode = Mode::Editor;
    Skybox* skybox;

    // Méthodes privées
    int initGLEW();
    GLFWwindow* initWindow();
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void processInputsGlobal(Camera* currentCamera, GLFWwindow* window);
    void scroll_callback(double xoffset, double yoffset);

public:
    // Constructeur et destructeur
    Mortar();
    ~Mortar();

    // Méthodes publiques
    int init();
    void loadScene();
    void gameCycle();
    void exit();
    const Mode& getCurrentMode() const;

    Camera* currentCamera;
    Camera* editorCamera;
    float initialWindowWidth;
    float initialWindowHeight;
};

#endif // MORTAR_HPP

