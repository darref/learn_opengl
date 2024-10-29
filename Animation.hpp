#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Skeleton.hpp"

struct Keyframe {
    float timeStamp;
    std::vector<glm::mat4> boneTransforms;
};

class Animation {
public:
    Animation(const std::string& filePath , const Skeleton& skeleton);
    const std::vector<Keyframe>& getKeyframes() const { return keyframes; }
    void loadFromFile(const std::string& filePath, const Skeleton& skeleton);
    float& getCurrentTime();
    float& getDuration();
    void update(float deltaTime);
    Keyframe getCurrentKeyframes() const ;
private:

    std::vector<Keyframe> keyframes;
    float currentTime = 0.0f;
    float animationDuration = 3.0f;

};

#endif // ANIMATION_HPP
