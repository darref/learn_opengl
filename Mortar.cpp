#include "Mortar.hpp"
#include <iostream>
#include <csignal>
#include <glm/gtc/matrix_transform.hpp>

Mortar::Mortar() {
    // Initialisation
}

Mortar::~Mortar() {
    // Nettoyage
}

int Mortar::initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Échec d'initialisation de GLEW" << std::endl;
        return -1;
    }
    return 0;
}

void Mortar::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Mortar* instance = static_cast<Mortar*>(glfwGetWindowUserPointer(window));
    if (instance == nullptr) {
        std::cerr << "Erreur: Instance de Mortar est nulle dans framebufferSizeCallback." << std::endl;
        return;
    }
    glViewport(0, 0, width, height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    instance->currentCamera->setAspectRatio(aspectRatio);
}

GLFWwindow* Mortar::initWindow() {
    if (!glfwInit()) {
        std::cerr << "Échec d'initialisation de GLFW" << std::endl;
        this->exit();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    int screenWidth = videoMode->width;
    int screenHeight = videoMode->height;
    initialWindowWidth = screenWidth * 0.9f;
    initialWindowHeight = screenHeight * 0.9f;

    GLFWwindow* window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, "|_| Mortar Engine |_|", nullptr, nullptr);
    if (!window) {
        std::cerr << "Échec de création de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        this->exit();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowPos(window, (screenWidth - initialWindowWidth) / 2.0f, (screenHeight - initialWindowHeight) / 2.0f);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        Mortar* game = static_cast<Mortar*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->framebufferSizeCallback(window, width, height);
        }
    });

    return window;
}

void Mortar::processInputsGlobal(Camera* currentCamera, GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && currentMode == Mode::Game) {
        currentMode = Mode::Editor;
        character->unpossess();
        currentCamera->firstMouse = false;
    } else if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && currentMode == Mode::Editor) {
        currentMode = Mode::Game;
        character->possess();
    }

    float deltaTime = fps->getDeltaTime();

    if (currentMode == Mode::Editor) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            currentCamera->moveForward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            currentCamera->moveBackward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            currentCamera->moveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            currentCamera->moveRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            currentCamera->turnLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            currentCamera->turnRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            currentCamera->moveUp(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            currentCamera->moveDown(deltaTime);

        currentCamera->processRightClickView(window, deltaTime);
    }
    else if (currentMode == Mode::Game)
    {
        character->processRightClickView(window, deltaTime);
    }
}

void Mortar::scroll_callback(double xoffset, double yoffset) {
    if (yoffset > 0) {
        std::cout << "Scrolling up!" << std::endl;
        currentCamera->increaseSpeed();
    } else if (yoffset < 0) {
        std::cout << "Scrolling down!" << std::endl;
        currentCamera->decreaseSpeed();
    }
}

int Mortar::init() {
    window = initWindow();
    if (!window) return -1;
    if (initGLEW() != 0) return -1;

    editorCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), 90.0f, 0.0f, 0.0f, 20.5f, 200.5f, 45.0f, (initialWindowWidth / initialWindowHeight), 0.1f, 1000000000.0f);
    currentCamera = editorCamera;
    fps = new FPSCounter(true);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glfwSwapInterval(0);
    shaderProgram = createShaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Mortar* game = static_cast<Mortar*>(glfwGetWindowUserPointer(window));
        game->scroll_callback(xoffset, yoffset);
    });

    frustum = new Frustum(currentCamera->getViewMatrix() * currentCamera->getProjectionMatrix());

    return 0;
}

void Mortar::loadScene() {
    terrain = new HeightmapTerrain("heightmaps/allBlackHeightmap.png", "textures/grid.jpg", 1.0f, 200.0f);
    terrain->recalculateBounds();

    for (int i = 0; i < 5; ++i) {
        auto* model = new Model(glm::vec3(i * 2.0f, 0.0f, 0.0f));
        model->init("textures/barret.png", "meshes/cube.glb");
        model->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        model->setPosition(glm::vec3(i * 2.0f, model->getSizeY() / 2.0f, 0.0f));
        model->setName("cubetest_" + std::to_string(i));
        models.push_back(model);
    }

    for (int i = 0; i < 5; ++i) {
        auto* model = new Model(glm::vec3(i * 2.0f, 2.0f, 0.0f));
        model->init("meshes/bodybuilder/bodybuilder.jpeg", "meshes/bodybuilder/bodybuilder.glb");
        model->setName("bodybuilder_" + std::to_string(i));
        model->setPosition(glm::vec3(i * 2.0f, model->getSizeY() / 2.0f, 3.0f));
        models.push_back(model);
    }

    character = new Character(glm::vec3(0.0f, 0.0f, 0.0f), "meshes/bodybuilder/bodybuilder.glb", "meshes/bodybuilder/bodybuilder.jpeg" , this);
    character->init();
    character->setPosition(glm::vec3(0.0f, character->getSizeY() / 2.0f , 5.0f));


}

void Mortar::gameCycle() {
    while (!glfwWindowShouldClose(window)) {
        fps->update();
        processInputsGlobal(currentCamera, window);

        if (currentMode == Mode::Game) {
            character->processInputs(window, fps->getDeltaTime());
            character->update(fps->getDeltaTime());
        }

        for (auto m : models) {
            m->animation(fps->getDeltaTime());
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = currentCamera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(currentCamera->FOV), currentCamera->aspectRatio, 0.1f, 1000.0f);
        frustum->setViewProjectionMatrix(projection * view);

        terrain->draw(shaderProgram, view, projection);
        for (auto m : models)
            m->draw(shaderProgram, view, projection);
        character->draw(shaderProgram, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Mortar::exit() {
    delete fps;
    delete currentCamera;
    delete terrain;
    for (auto m : models)
        delete m;
    delete character;

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    std::raise(SIGKILL);
}

const Mode& Mortar::getCurrentMode() const
{
    return currentMode;
}
