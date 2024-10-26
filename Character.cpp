#include "Character.hpp"
#include "Mortar.hpp"
#include <glm/gtx/string_cast.hpp>


Character::Character(const glm::vec3& startPos , const string& modelFilePath, const string& textureFilePath , Mortar* game)
    : position(startPos), velocity(0.0f), gravity(-9.8f), moveSpeed(5.0f), jumpStrength(2.5f), groundLevel(startPos.y),
      model(Model(startPos)) , game(game) 
{
    model.init(textureFilePath,modelFilePath);
    characterCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), 90.0f, 0.0f, 0.0f, 20.5f, 200.5f, 45.0f, (game->initialWindowWidth / game->initialWindowHeight), 0.1f, 1000000000.0f);
}

void Character::init()
{
    placeCameraInBack();
}

void Character::update(float deltaTime) {
    applyGravity(deltaTime);
    position += velocity * deltaTime;
    model.setPosition(position);

    if(position.y <= groundLevel + getSizeY() /2.0f)
    {
        position.y <= groundLevel + getSizeY() /2.0f;
        isJumping = false;
        velocity.y = 0.0f;
    }

    //std::cout << pitch << std::endl;

}

void Character::placeCameraInBack() {
    glm::vec3 characterPosition = this->getPosition();
    glm::vec3 characterDirection = this->getDirection();

    // Positionner la caméra derrière le personnage
    glm::vec3 cameraPosition = characterPosition - characterDirection * springArmLength;
    characterCamera->setPosition(cameraPosition);

    // Définir la direction de la caméra comme celle du personnage
    characterCamera->setDirection(characterDirection);

    // Débogage
    //std::cout << "Character Position: " << glm::to_string(characterPosition) << std::endl;
    //std::cout << "Character Direction: " << glm::to_string(characterDirection) << std::endl;
    //std::cout << "Camera Position : " << glm::to_string(cameraPosition) << std::endl;
    //std::cout << "Camera Direction: " << glm::to_string(characterCamera->getDirection()) << std::endl;
}






void Character::moveForward(float deltaTime) {
    position += getDirection() * moveSpeed * deltaTime;
}

void Character::moveBackward(float deltaTime) {
    position += -getDirection() * moveSpeed * deltaTime;
}

void Character::moveLeft(float deltaTime) {
    position += -getRightVector() * moveSpeed * deltaTime;
}

void Character::moveRight(float deltaTime) {
    position += getRightVector() * moveSpeed * deltaTime;
}

void Character::applyGravity(float deltaTime) {
    // Applique la gravité continuellement
    if(isJumping)
        velocity.y += gravity * deltaTime;

}

void Character::jump(float strength) {
    // Permet de sauter uniquement si le personnage est au sol
    if (!isJumping ) {
        velocity.y = jumpStrength * strength;
        isJumping = true; // Indique que le personnage est en train de sauter
    }
}

void Character::processMouseMovement(float xOffset, float yOffset, float deltaTime) {
    xOffset *= turnSpeed * deltaTime;
    yOffset *= turnSpeed * deltaTime;

    // Rotation en yaw du personnage
    glm::vec3 rot = model.getRotation();
    float yaw = rot.y;

    // Mise à jour du yaw pour le personnage
    yaw -= xOffset;

    // Appliquer la rotation en yaw uniquement au personnage
    model.setRotation(glm::vec3(0.0f, yaw, 0.0f));

    // Calcul du pitch pour la caméra (indépendant de la rotation du personnage)
    pitch -= yOffset;

    // Limiter le pitch pour éviter de trop regarder en haut ou en bas
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < 0.0f) pitch = 0.0f;

    // Position de base du personnage
    glm::vec3 characterPosition = model.getPosition();

    // Calculer la position de la caméra autour du personnage en utilisant yaw et pitch
    float horizontalDistance = springArmLength * cos(glm::radians(pitch));
    float verticalDistance = springArmLength * sin(glm::radians(pitch));

    // Déplacer la caméra autour du personnage selon yaw et pitch
    float offsetX = horizontalDistance * sin(glm::radians(yaw));
    float offsetZ = horizontalDistance * cos(glm::radians(yaw));

    glm::vec3 cameraPosition = characterPosition + glm::vec3(offsetX, verticalDistance, offsetZ);
    characterCamera->setPosition(cameraPosition);

    // Faire regarder la caméra vers le personnage
    glm::vec3 cameraDirection = glm::normalize(characterPosition - cameraPosition);
    characterCamera->setDirection(cameraDirection);
}


void Character::processRightClickView(GLFWwindow* window, float deltaTime)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Obtenir la position actuelle de la souris
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Initialiser lastX et lastY au premier mouvement de souris
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        // Calcul des offsets de mouvement de souris
        float xOffset = xpos - lastX;
        float yOffset = ypos - lastY;

        // Mise à jour de lastX et lastY
        lastX = xpos;
        lastY = ypos;

        // Appel de la fonction de traitement du mouvement de souris
        processMouseMovement(xOffset, yOffset, deltaTime);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true; // Réinitialiser pour la prochaine fois
        replaceCamera();
    }
}

void Character::replaceCamera()
{
    glm::vec3 rot = model.getRotation();
    float yaw = rot.y;
    // Limiter le pitch pour éviter de trop regarder en haut ou en bas
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    // Position de base du personnage
    glm::vec3 characterPosition = model.getPosition();
    // Calculer la position de la caméra autour du personnage en utilisant yaw et pitch
    float horizontalDistance = springArmLength * cos(glm::radians(pitch));
    float verticalDistance = springArmLength * sin(glm::radians(pitch));
    // Déplacer la caméra autour du personnage selon yaw et pitch
    float offsetX = horizontalDistance * sin(glm::radians(yaw));
    float offsetZ = horizontalDistance * cos(glm::radians(yaw));

    glm::vec3 cameraPosition = characterPosition + glm::vec3(offsetX, verticalDistance, offsetZ);
    characterCamera->setPosition(cameraPosition);
    // Faire regarder la caméra vers le personnage
    glm::vec3 cameraDirection = glm::normalize(characterPosition - cameraPosition);
    characterCamera->setDirection(cameraDirection);
}

glm::vec3 Character::getPosition() const {
    return position;
}

glm::vec3 Character::getDirection() const
{
    return model.getDirection();
}
glm::vec3 Character::getRightVector() const
{
    return model.getRightVector();
}
Model& Character::getModel() {
    return model;
}

float Character::getSizeX() const
{
    return model.getSizeX();
}
float Character::getSizeY() const
{
    return model.getSizeY();
}
float Character::getSizeZ() const
{
    return model.getSizeZ();
}
glm::vec3 Character::getBoundingBoxMin() const
{
    return model.getBoundingBoxMin();
}
glm::vec3 Character::getBoundingBoxMax() const
{
    return model.getBoundingBoxMax();
}
void Character::setPosition(glm::vec3 pos)
{
    position = pos;
    model.setPosition(pos);
}
void Character::possess()
{
    game->currentCamera = characterCamera;
}
void Character::unpossess()
{
    game->currentCamera = game->editorCamera;
}

void Character::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection)
{
    frustum.setViewProjectionMatrix( projection * view );

    glm::vec3 minC = this->getModel().getBoundingBoxMin();
    glm::vec3 maxC = this->getModel().getBoundingBoxMax();
    if(!frustum.isBoxInFrustum(minC, maxC))
        return;
    model.draw(shaderProgram , view , projection);
}

void Character::processInputs(GLFWwindow* window , float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        this->moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        this->moveBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        this->moveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        this->moveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        this->jump(2.0f);

}
