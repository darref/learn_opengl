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
#include "HeightmapTerrain.hpp"
#include <csignal>  // pour utiliser std::raise()
 


class Mortar
{
    private:
        FPSCounter* fps;
        Camera* camera;
        GLFWwindow* window;
        GLuint shaderProgram;
        Skybox* skybox;
        HeightmapTerrain* terrain ;
        std::vector<Model*> models;
        float initialWindowWidth , initialWindowHeight;
    public:
        
    int initGLEW() {
        // Initialisation de GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cout << "Échec d'initialisation de GLEW" << std::endl;
            return -1;
        }
        return 0;
    }

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) 
    {
        // Récupérer le pointeur de l'instance de la classe Mortar
        Mortar* instance = static_cast<Mortar*>(glfwGetWindowUserPointer(window));

        // Vérifiez que l'instance est valide
        if (instance == nullptr) {
            std::cerr << "Erreur: Instance de Mortar est nulle dans framebufferSizeCallback." << std::endl;
            return;
        }

        // Ajuster le viewport
        glViewport(0, 0, width, height);

        // Calculer l'aspect ratio (rapport largeur/hauteur)
        float aspectRatio =  static_cast<float>(width) / static_cast<float>(height);
        instance->camera->setAspectRatio(aspectRatio);
        
        
    }

    GLFWwindow* initWindow() 
    {
        // Initialisation de GLFW
        if (!glfwInit()) {
            std::cerr << "Échec d'initialisation de GLFW" << std::endl;
            this->exit();
        }

        // Création de la fenêtre
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Get the primary monitor
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (!primaryMonitor) {
            std::cerr << "Failed to get primary monitor" << std::endl;
            glfwTerminate();
            this->exit();
        }

        // Get video mode of the primary monitor
        const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
        if (!videoMode) {
            std::cerr << "Failed to get video mode" << std::endl;
            glfwTerminate();
            this->exit();
        }

        // Get screen width and height
        int screenWidth = videoMode->width;
        int screenHeight = videoMode->height;
        initialWindowWidth = screenWidth*0.8f;
        initialWindowHeight = screenHeight*0.8f;
        
        GLFWwindow* window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, "|_| Mortar Engine |_|", nullptr, nullptr);
        if (!window) {
            std::cerr << "Échec de création de la fenêtre GLFW" << std::endl;
            glfwTerminate();
            this->exit();
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Activer le swap interval pour la synchronisation verticale
        glfwSetWindowPos(window, (screenWidth - initialWindowWidth)/2.0f, (screenHeight-initialWindowHeight)/2.0f);
        

        // Stocke un pointeur sur l'instance de la classe actuelle
        glfwSetWindowUserPointer(window, this);

        // Utiliser une lambda pour faire appel à la méthode d'instance
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            Mortar* game = static_cast<Mortar*>(glfwGetWindowUserPointer(window));
            if (game) {
                game->framebufferSizeCallback(window, width, height);  // Appelle la méthode de la classe
            }
        });

        return window;
    }


    void processInputs(Camera* camera , GLFWwindow* window )
    {
         if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }

        float deltaTime = fps->getDeltaTime();  // Use the delta time for smooth movement

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->moveForward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->moveBackward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera->moveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera->moveRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->turnLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->turnRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera->moveUp(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->moveDown(deltaTime);


        // Mouse input handling for camera rotation
        camera->processRightClickView(window , deltaTime);
    }

    void scroll_callback(double xoffset, double yoffset ) {
        // xoffset représente le mouvement horizontal de la molette (rarement utilisé)
        // yoffset représente le mouvement vertical (généralement utilisé pour zoomer/dézoomer)
        if (yoffset > 0) {
            std::cout << "Scrolling up!" << std::endl;
            camera->increaseSpeed();
        } else if (yoffset < 0) {
            std::cout << "Scrolling down!" << std::endl;
            camera->decreaseSpeed();
        }
    }

    int init() {
        glfwSetWindowUserPointer(window, this);

        window = initWindow();
        if (!window) return -1; // Vérifier si la fenêtre est bien créée
        if (initGLEW() != 0) return -1; // Vérifier si GLEW est bien initialisé

        camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), 90.0f, 0.0f, 0.0f, 20.5f, 200.5f, 45.0f, (initialWindowWidth / initialWindowHeight) , 0.1f, 1000000000.0f);
        fps = new FPSCounter(true);
        // Stocke un pointeur sur l'instance de la classe actuelle

        glEnable(GL_DEPTH_TEST);
        
        glClearColor(0.4f, 0.0f, 1.0f, 1.0f); // Noir
        glDisable(GL_CULL_FACE); // Désactiver le culling
        glfwSwapInterval(0);
        shaderProgram = createShaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

        // Utiliser une lambda pour faire appel à la méthode d'instance
        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
            Mortar* game = static_cast<Mortar*>(glfwGetWindowUserPointer(window));
            game->scroll_callback(xoffset, yoffset);  // Appelle la méthode de la classe
        });
        return 0;
    }

    void loadScene()
    {
        skybox = new Skybox(glm::vec3(0.0f, 0.0f, 0.0f) , 100000.0f);
        skybox->init("textures/skybox2.png");
        terrain = new HeightmapTerrain("heightmaps/allBlackHeightmap.png","textures/grid.jpg"  , 1.0f ,200.0f);
        
        for(int i = 0; i < 5; ++i)
        {
            models.push_back( new Model(glm::vec3(i*2,0.0f,0.0f)) );
            models.back()->init("textures/barret.png" , "meshes/cube.glb");
            //models.back()->setRotation(glm::vec3(-90.0f , 0.0f , 0.0f ));
            models.back()->setScale(glm::vec3(0.5f,0.5f,0.5f ));
            //models.back()->setPosition(glm::vec3(0.0f,25.0f,0.0f ));  

        }
        for(int i = 0; i < 5; ++i)
        {
            models.push_back( new Model(glm::vec3(i*2,2.0f,0.0f)) );
            models.back()->init("meshes/bodybuilder/bodybuilder.jpeg" , "meshes/bodybuilder/bodybuilder.glb");
            //models.back()->setRotation(glm::vec3(-90.0f , 0.0f , 0.0f ));
            //models.back()->setScale(glm::vec3(0.01f,0.01f,0.01f ));
            //models.back()->setPosition(glm::vec3(0.0f,25.0f,0.0f ));        
        }

    }

    void gameCycle()
    {
        while (!glfwWindowShouldClose(window)) 
        {
            fps->update();
            // Entrées utilisateur
            processInputs(camera , window );
            // Effacer l'écran
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 view = camera->getViewMatrix();
            glm::mat4 projection = camera->getProjectionMatrix();
            // Dessiner les cubes
            skybox->draw(shaderProgram, view, projection);
            terrain->draw(shaderProgram, view, projection);
            for(auto m : models)
            {
                m->animation(fps->deltaTime); // Assurez-vous que cette méthode est implémentée
                m->draw(shaderProgram, view, projection);
            }
            // Swap buffers et poll events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    void exit()
    {
        delete fps;
        delete camera;
        delete skybox;
        delete terrain ;
        // Libération des ressources
        for(auto m : models)
            delete m; // Libérer la mémoire allouée pour le cube
        glDeleteProgram(shaderProgram);
        glfwTerminate();
        std::raise(SIGKILL);
    }
};

int main() {
    Mortar game;
    game.init();
    game.loadScene();
    game.gameCycle();
    game.exit();
    return 0;
}

