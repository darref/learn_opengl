#include "ObjectBase.hpp"

// Constructeur par défaut
ObjectBase::ObjectBase() : position(0.0f, 0.0f, 0.0f) {}

// Méthode pour définir la position
void ObjectBase::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

// Méthode pour obtenir la position
glm::vec3 ObjectBase::getPosition() const {
    return position;
}
