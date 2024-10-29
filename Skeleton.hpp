#ifndef SKELETON_HPP
#define SKELETON_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Bone {
    std::string name;
    int parentIndex;
    glm::mat4 offsetMatrix;
};

class Skeleton {
public:
    Skeleton(){};
    Skeleton(const std::string& filePath);

    const std::vector<Bone>& getBones() const { return bones; }
    void loadFromFile(const std::string& filePath);
    bool isEmpty();
private:

    std::vector<Bone> bones;
};

#endif // SKELETON_HPP
