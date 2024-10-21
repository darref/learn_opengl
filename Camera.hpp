#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    // Position and orientation
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // Camera parameters
    float yaw;
    float pitch;
    float roll;
    float speed;
    float turnSpeed;
    float nearPlane;
    float farPlane;
    float FOV;
    float aspectRatio;
    bool firstMouse;
    float lastX, lastY; 

    // Constructor
    Camera(glm::vec3 startPosition, float startYaw, float startPitch, float startRoll,
           float startSpeed, float startTurnSpeed, float startFOV, 
           float startAspectRatio, float startNear, float startFar) 
        : position(startPosition), 
          front(glm::vec3(0.0f, 0.0f, -1.0f)), // Initialize front vector
          up(glm::vec3(0.0f, 1.0f, 0.0f)),      // Initialize up vector
          yaw(startYaw), 
          pitch(startPitch), 
          roll(startRoll),        
          speed(startSpeed), 
          turnSpeed(startTurnSpeed),
          nearPlane(startNear), 
          farPlane(startFar),
          FOV(startFOV), 
          aspectRatio(startAspectRatio) 
    {
        updateCameraVectors(); // Update camera vectors after initialization
    }


    // Member functions
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);
    void turnLeft(float deltaTime);
    void turnRight(float deltaTime);
    void turnUp(float deltaTime);
    void turnDown(float deltaTime);
    void processMouseMovement(float xOffset, float yOffset, float deltaTime);
    void updateCameraVectors();
    void processRightClickView(GLFWwindow* window, float deltaTime);
};

#endif // CAMERA_HPP

