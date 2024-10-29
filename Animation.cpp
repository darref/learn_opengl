#include "Animation.hpp"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <unordered_map>




Animation::Animation(const std::string& filePath , const Skeleton& skeleton) {
    loadFromFile(filePath , skeleton);
}

void Animation::loadFromFile(const std::string& filePath, const Skeleton& skeleton) {
    Assimp::Importer importer;

    // Charger le fichier avec Assimp
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasAnimations()) {
        std::cerr << "Erreur : Aucun fichier d'animation trouvé dans " << filePath << std::endl;
        return;
    }

    // On récupère la première animation de la scène
    aiAnimation* animation = scene->mAnimations[0];
    keyframes.clear();

    // Map pour vérifier les os présents dans le squelette
    std::unordered_map<std::string, int> skeletonBones;
    for (size_t i = 0; i < skeleton.getBones().size(); i++) {
        skeletonBones[skeleton.getBones()[i].name] = i;
    }

    // Parcourir les canaux d'animation et filtrer ceux qui correspondent aux os du squelette
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* channel = animation->mChannels[i];

        // Vérifier si le nom de l'os du canal d'animation est dans le squelette
        if (skeletonBones.find(channel->mNodeName.C_Str()) == skeletonBones.end()) {
            std::cout << "Os ignoré dans l'animation : " << channel->mNodeName.C_Str() << std::endl;
            continue;
        }

        int boneIndex = skeletonBones[channel->mNodeName.C_Str()];

        // Parcourir chaque keyframe de position pour cet os
        for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
            Keyframe keyframe;
            keyframe.timeStamp = static_cast<float>(channel->mPositionKeys[j].mTime);

            // Initialiser la transformation des os avec la taille du nombre d'os dans le squelette
            keyframe.boneTransforms.resize(skeleton.getBones().size(), glm::mat4(1.0f));

            // Convertir la position, rotation et échelle en matrices
            aiVector3D position = channel->mPositionKeys[j].mValue;
            aiQuaternion rotation = channel->mRotationKeys[j].mValue;
            aiVector3D scale = channel->mScalingKeys[j].mValue;

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
            glm::mat4 rotationMat = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

            // Multiplication des matrices pour obtenir la transformation finale
            keyframe.boneTransforms[boneIndex] = translation * rotationMat; //* scaleMat;

            // Ajouter ou mettre à jour le keyframe dans le vecteur
            if (j >= keyframes.size()) {
                keyframes.push_back(keyframe);
            } else {
                keyframes[j].boneTransforms[boneIndex] = keyframe.boneTransforms[boneIndex];
            }
        }
    }

    std::cout << "Chargement de l'animation depuis " << filePath << " réussi. Nombre de keyframes : " << keyframes.size() << std::endl;
    std::cout << "L'animation a lu " << skeleton.getBones().size() << " os dans le squelette." << std::endl;
}




float& Animation::getCurrentTime()
{
	return currentTime;
}
float& Animation::getDuration()
{
	return animationDuration;
}
void Animation::update(float deltaTime)
{

    // Update the animation time
    currentTime += deltaTime;
    if (currentTime > animationDuration) {
        currentTime = fmod(currentTime, animationDuration); // Loop the animation
    }
}

// Utility function to interpolate between two transformation matrices
static glm::mat4 interpolateMatrices(const glm::mat4& mat1, const glm::mat4& mat2, float mixFactor) {
    glm::vec3 scale1, translation1, skew1;
    glm::quat rotation1;
    glm::vec4 perspective1;
    glm::decompose(mat1, scale1, rotation1, translation1, skew1, perspective1);

    glm::vec3 scale2, translation2, skew2;
    glm::quat rotation2;
    glm::vec4 perspective2;
    glm::decompose(mat2, scale2, rotation2, translation2, skew2, perspective2);

    glm::vec3 interpolatedTranslation = glm::mix(translation1, translation2, mixFactor);
    glm::quat interpolatedRotation = glm::slerp(rotation1, rotation2, mixFactor);
    glm::vec3 interpolatedScale = glm::mix(scale1, scale2, mixFactor);

    return glm::translate(interpolatedTranslation) *
           glm::toMat4(interpolatedRotation) *
           glm::scale(interpolatedScale);
}

Keyframe Animation::getCurrentKeyframes() const {
    if (keyframes.empty()) {
        std::cerr << "Erreur : Aucun keyframe trouvé dans l'animation." << std::endl;
        return Keyframe();
    }

    Keyframe previousKeyframe, nextKeyframe;
    bool found = false;

    for (size_t i = 0; i < keyframes.size() - 1; i++) {
        if (currentTime >= keyframes[i].timeStamp && currentTime <= keyframes[i + 1].timeStamp) {
            previousKeyframe = keyframes[i];
            nextKeyframe = keyframes[i + 1];
            found = true;
            break;
        }
    }

    if (!found) {
        return keyframes.back();
    }

    float interpolationFactor = (currentTime - previousKeyframe.timeStamp) / (nextKeyframe.timeStamp - previousKeyframe.timeStamp);
    Keyframe interpolatedKeyframe;
    interpolatedKeyframe.timeStamp = currentTime;

    for (size_t j = 0; j < previousKeyframe.boneTransforms.size(); j++) {
        glm::mat4 interpolatedTransform = interpolateMatrices(previousKeyframe.boneTransforms[j], nextKeyframe.boneTransforms[j], interpolationFactor);
        interpolatedKeyframe.boneTransforms.push_back(interpolatedTransform);
    }

    return interpolatedKeyframe;
}

