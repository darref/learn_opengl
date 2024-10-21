#include <iostream>
#include <GLFW/glfw3.h> // Assurez-vous que cette bibliothèque est bien incluse pour glfwGetTime()

class FPSCounter {
private:

public:
    float deltaTime;
    float lastFrame;
    float currentFrame;
    float fps;
    float accumulatedTime;
    int frameCount;
    bool showFPS;
    FPSCounter(bool showFPS = true) 
        : deltaTime(0.0f), lastFrame(0.0f), currentFrame(0.0f), fps(0.0f),
          accumulatedTime(0.0f), frameCount(0), showFPS(showFPS) {}

    void update() {
        currentFrame = glfwGetTime(); // Obtenir le temps actuel
        deltaTime = currentFrame - lastFrame; // Calculer le temps écoulé depuis le dernier frame
        lastFrame = currentFrame; // Mettre à jour lastFrame

        accumulatedTime += deltaTime; // Accumuler le temps écoulé
        frameCount++; // Incrémenter le compteur de frames

        // Calculer les FPS toutes les 0.5 secondes
        if (accumulatedTime >= 0.5f) {
            fps = frameCount / accumulatedTime; // Calculer les FPS

            if (showFPS) {
                std::cout << "FPS = " << fps << std::endl; // Afficher les FPS
            }

            // Réinitialiser les variables pour la prochaine mesure
            accumulatedTime = 0.0f;
            frameCount = 0;
        }
    }

    float getDeltaTime() const {
        return deltaTime;
    }

    float getFPS() const {
        return fps;
    }

    void toggleFPSDisplay() {
        showFPS = !showFPS;
    }
};

