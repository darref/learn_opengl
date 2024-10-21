// Inclure les bibliothèques nécessaires
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>   // Pour charger les images
#include <glm/glm.hpp>   // Pour les transformations
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include "FPSCounter.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Shaders.h"
#include "Window.h"

int initGLEW() {
    // Initialisation de GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Échec d'initialisation de GLEW" << std::endl;
        return -1;
    }
    return 0;
}

void processInputs(Camera& camera , GLFWwindow* window , const FPSCounter& fps)
{
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

    float deltaTime = fps.getDeltaTime();  // Use the delta time for smooth movement

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.moveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.moveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.turnLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.turnRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveUp(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.moveDown(deltaTime);


    // Mouse input handling for camera rotation
    camera.processRightClickView(window , deltaTime);
}

int main() {
    FPSCounter fps(true);
    GLFWwindow* window = initWindow();
    if (!window) return -1; // Vérifier si la fenêtre est bien créée
    if (initGLEW() != 0) return -1; // Vérifier si GLEW est bien initialisé
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.4f, 0.0f, 1.0f, 1.0f); // Noir
    glDisable(GL_CULL_FACE); // Désactiver le culling
    glfwSwapInterval(0);
    GLuint shaderProgram = createShaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");


    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), 90.0f, 0.0f,0.0f ,  20.5f, 200.5f, 45.0f, 800.0f / 600.0f, 0.1f, 1000000000.0f);


    Skybox* skybox = new Skybox(glm::vec3(0.0f, 0.0f, 0.0f) , 100000.0f);
    skybox->init("textures/skybox2.png");
    std::vector<Model*> models;
    for(int i = 0; i < 10; ++i)
    {
        models.push_back( new Model(glm::vec3(i*2,0.0f,0.0f)) );
        models.back()->init("textures/barret.png" , "meshes/cube.glb");
        //models.back()->setRotation(glm::vec3(-90.0f , 0.0f , 0.0f ));
        models.back()->setScale(glm::vec3(0.5f,0.5f,0.5f ));

    }
    for(int i = 0; i < 10 ; ++i)
    {
        models.push_back( new Model(glm::vec3(i*2,2.0f,0.0f)) );
        models.back()->init("meshes/bodybuilder/bodybuilder.jpeg" , "meshes/bodybuilder/bodybuilder.glb");
        //models.back()->setRotation(glm::vec3(-90.0f , 0.0f , 0.0f ));
        //models.back()->setScale(glm::vec3(0.01f,0.01f,0.01f ));
    }

    while (!glfwWindowShouldClose(window)) 
    {
        fps.update();
        // Entrées utilisateur
        processInputs(camera , window , fps);
        // Effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        // Dessiner les cubes
        skybox->draw(shaderProgram, view, projection);
        for(auto m : models)
        {
            m->animation(fps.deltaTime); // Assurez-vous que cette méthode est implémentée
            m->draw(shaderProgram, view, projection);
        }
        // Swap buffers et poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Libération des ressources
    for(int i = 0; i < 20 ; ++i)
        delete models[i]; // Libérer la mémoire allouée pour le cube
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

