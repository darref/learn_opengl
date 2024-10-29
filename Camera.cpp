#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
}
const glm::vec3& Camera::getPosition()
{
    return position;
}
glm::vec3 Camera::getRotation() const
{
    return glm::vec3(pitch , yaw , roll);
}

void Camera::setAspectRatio(float ar)
{
    this->aspectRatio = ar;    
}

void Camera::moveForward(float deltaTime) {
    position += front * speed * deltaTime;
}

void Camera::moveBackward(float deltaTime) {
    position -= front * speed * deltaTime;
}

void Camera::moveLeft(float deltaTime) {
    position -= glm::normalize(glm::cross(front, up)) * speed * deltaTime;
}

void Camera::moveRight(float deltaTime) {
    position += glm::normalize(glm::cross(front, up)) * speed * deltaTime;
}

void Camera::moveUp(float deltaTime) {
    position += up * speed * deltaTime;
}

void Camera::moveDown(float deltaTime) {
    position -= up * speed * deltaTime;
}

void Camera::turnLeft(float deltaTime) {
    yaw -= turnSpeed * deltaTime; // Tourner vers la gauche
    updateCameraVectors(); // Mettre à jour les vecteurs de la caméra
}

void Camera::turnRight(float deltaTime) {
    yaw += turnSpeed * deltaTime; // Tourner vers la droite
    updateCameraVectors(); // Mettre à jour les vecteurs de la caméra
}

void Camera::turnUp(float deltaTime) {
    roll += turnSpeed * deltaTime; // Lever la caméra
    if (roll > 89.0f) { // Limiter la rotation pour éviter les artefacts visuels
        roll = 89.0f;
    }
    updateCameraVectors(); // Mettre à jour les vecteurs de la caméra
}

void Camera::turnDown(float deltaTime) {
    roll -= turnSpeed * deltaTime; // Abaisser la caméra
    if (roll < -89.0f) { // Limiter la rotation
        roll = -89.0f;
    }
    updateCameraVectors(); // Mettre à jour les vecteurs de la caméra
}

void Camera::processMouseMovement(float xOffset, float yOffset , float deltaTime) {
    xOffset *= turnSpeed * deltaTime;
    yOffset *= turnSpeed * deltaTime;

    // Constrain the pitch to avoid looking too far up or down (between -89 and +89 degrees)
    yaw += xOffset;
    pitch -= yOffset; // Inverted mouse Y-axis (moving up should decrease pitch)

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update the camera vectors based on the new yaw and pitch, without changing roll
    updateCameraVectors();
}


void Camera::updateCameraVectors() {
    // Calculate the new direction based on yaw and pitch
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    // Right vector remains perpendicular to the global up vector (0, 1, 0)
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Update the up vector as well, but keep it perpendicular to front and right (no roll)
    up = glm::normalize(glm::cross(right, front));
}

void Camera::processRightClickView(GLFWwindow* window, float deltaTime)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // Hide the cursor and capture it in the window
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Get the current mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // If this is the first mouse input, initialize lastX and lastY to the current cursor position
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false; // Prevents resetting on the first input
        }

        // Calculate the offset for mouse movement
        float xOffset = xpos - lastX; // Horizontal movement
        float yOffset = ypos - lastY; // Vertical movement (inverted)

        // Update lastX and lastY
        lastX = xpos;
        lastY = ypos;

        // Process mouse movement to rotate the camera without resetting pitch or yaw
        processMouseMovement(xOffset, yOffset , deltaTime);
    }
    else {
        // When the right-click is released, show the cursor again
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true; // Reset the flag to allow reinitialization on the next click
    }
}

void Camera::decreaseSpeed()
{
    speed = speed < minSpeed ? minSpeed : speed *0.8f;
}

void Camera::increaseSpeed()
{
    speed = speed > maxSpeed ? maxSpeed : speed *1.2f;
}
// Définit la position de la caméra
void Camera::setPosition(const glm::vec3& newPosition) {
    position = newPosition; // Met à jour la position de la caméra
    updateCameraVectors();
}

void Camera::setDirection(const glm::vec3& newDirection) {
    front = glm::normalize(newDirection);

    // Calculer yaw et pitch à partir de la direction donnée
    pitch = glm::degrees(asin(front.y));  // Utilise l'axe Y pour le pitch
    yaw = glm::degrees(atan2(front.z, front.x));  // Utilise X et Z pour le yaw

    // Réajuste les vecteurs up et right en fonction de la nouvelle direction
    updateCameraVectors();
}

void Camera::lookAt(const ObjectBase& obj)
{
    glm::vec3 cameraDirection = glm::normalize(obj.getPosition() - this->position);
    this->setDirection(cameraDirection);
}
void Camera::lookAt(const glm::vec3& pos)
{
    glm::vec3 cameraDirection = glm::normalize(pos - this->position);
    this->setDirection(cameraDirection);
}

void Camera::setRotation(const glm::vec3& newRotation)
{
    this->yaw = newRotation.y;
    this->pitch = newRotation.x;
    this->roll = newRotation.z;
    updateCameraVectors();
}

glm::vec3 Camera::getDirection() const 
{
    return front;
}
