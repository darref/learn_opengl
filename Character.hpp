#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Model.hpp"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Frustum.hpp"
#include "Camera.hpp"
#include "ObjectBase.hpp"
class Mortar;

class Character : public ObjectBase{
public:
    Character(const glm::vec3& startPos , const string& modelFilePath, const string& textureFilePath, Mortar* game);
    void init();
    void loadAnimation(const string& animFilePath , const string& animationName);

    void update(float deltaTime);
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void applyGravity(float deltaTime);
    void jump(float strength);
    void processMouseMovement(float xOffset, float yOffset , float deltaTime);
    void processRightClickView(GLFWwindow* window, float deltaTime);

    glm::vec3 getPosition() const;
    Model& getModel();
    glm::vec3 getDirection() const;
    glm::vec3 getRightVector() const;
    float getSizeX() const;
    float getSizeY() const;
    float getSizeZ() const;
    glm::vec3 getBoundingBoxMin() const;
    glm::vec3 getBoundingBoxMax() const;
    void setPosition(glm::vec3 pos);
    void setAnimation(const std::string& animationType);

    void possess();
    void unpossess();
    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);
    void processInputs(GLFWwindow* window , float deltaTime);
    void placeCameraInBack();
    void replaceCamera();
private:
    glm::vec3 position;
    glm::vec3 velocity;
    float gravity;
    float moveSpeed;
    float jumpStrength;
    float groundLevel;
    Model model;
    Frustum frustum;
    Camera* characterCamera;
    Mortar* game;
    float springArmLength = 5.0f;
    float lastX, lastY; 
    float turnSpeed = 20.5f;
    bool firstMouse = true;
    float pitch;
    bool isJumping = false;
};

#endif
