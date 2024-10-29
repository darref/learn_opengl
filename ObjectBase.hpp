#ifndef ObjectBase_HPP
#define ObjectBase_HPP

#include <glm/glm.hpp>

class ObjectBase {
public:
    // Constructeur par défaut
    ObjectBase();

    // Méthode pour définir la position
    void setPosition(const glm::vec3& newPosition);

    // Méthode pour obtenir la position
    glm::vec3 getPosition() const;

protected:
    glm::vec3 position; // Attribut de position
};

#endif // ObjectBase_HPP

