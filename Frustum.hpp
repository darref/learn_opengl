#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include <glm/glm.hpp>
#include <array>

class Frustum {
public:
    Frustum();
    Frustum(const glm::mat4& viewProjectionMatrix);
    bool isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const;
    void setViewProjectionMatrix(const glm::mat4& vpm);
private:
    void extractPlanes(const glm::mat4& viewProjectionMatrix);

    struct Plane {
        glm::vec3 normal;
        float distance;

        float distanceToPoint(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };

    std::array<Plane, 6> planes; // 6 plans pour le frustum
};

#endif

