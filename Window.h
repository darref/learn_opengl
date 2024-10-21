void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Adjust the OpenGL viewport based on the new window size
    glViewport(0, 0, width, height);
}

GLFWwindow* initWindow() {
    // Initialisation de GLFW
    if (!glfwInit()) {
        std::cout << "Échec d'initialisation de GLFW" << std::endl;
        return NULL;
    }

    // Création de la fenêtre
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube Texturé", NULL, NULL);
    if (!window) {
        std::cout << "Échec de création de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Activer le swap interval pour la synchronisation verticale

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    return window;
}
