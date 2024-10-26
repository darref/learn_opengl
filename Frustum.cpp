#include "Frustum.hpp"

Frustum::Frustum() {
    
}

Frustum::Frustum(const glm::mat4& viewProjectionMatrix) {
    extractPlanes(viewProjectionMatrix);
}

void Frustum::extractPlanes(const glm::mat4& viewProjectionMatrix) {
    planes[0].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
    planes[0].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
    planes[0].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
    planes[0].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];

    planes[1].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
    planes[1].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
    planes[1].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
    planes[1].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];

    planes[2].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
    planes[2].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
    planes[2].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
    planes[2].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];

    planes[3].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
    planes[3].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
    planes[3].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
    planes[3].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];

    planes[4].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
    planes[4].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
    planes[4].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
    planes[4].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];

    planes[5].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
    planes[5].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
    planes[5].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
    planes[5].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

    for (auto& plane : planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

void Frustum::setViewProjectionMatrix(const glm::mat4& vpm)
{
    extractPlanes(vpm);
}

bool Frustum::isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const {
    for (const auto& plane : planes) {
        glm::vec3 positiveVertex = min;
        glm::vec3 negativeVertex = max;

        if (plane.normal.x >= 0) {
            positiveVertex.x = max.x;
            negativeVertex.x = min.x;
        } else {
            positiveVertex.x = min.x;
            negativeVertex.x = max.x;
        }

        if (plane.normal.y >= 0) {
            positiveVertex.y = max.y;
            negativeVertex.y = min.y;
        } else {
            positiveVertex.y = min.y;
            negativeVertex.y = max.y;
        }

        if (plane.normal.z >= 0) {
            positiveVertex.z = max.z;
            negativeVertex.z = min.z;
        } else {
            positiveVertex.z = min.z;
            negativeVertex.z = max.z;
        }

        // Si le point le plus proche est derrière le plan, la box est hors frustum
        if (plane.distanceToPoint(positiveVertex) < 0) {
            return false;
        }
    }
    return true;
}


