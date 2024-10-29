#include "Skeleton.hpp"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Skeleton::Skeleton(const std::string& filePath) {
    loadFromFile(filePath);
}

bool Skeleton::isEmpty()
{
  return bones.empty();
}

void Skeleton::loadFromFile(const std::string& filePath) {
    Assimp::Importer importer;

    // Charger le fichier avec Assimp
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Erreur : Impossible de charger les os depuis le fichier " << filePath << std::endl;
        return;
    }

    // Clear previous bones if any
    bones.clear();

    // Parcourir chaque os dans le premier mesh (généralement les os sont partagés)
    aiMesh* mesh = scene->mMeshes[0];
    if (mesh->HasBones()) {
        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            aiBone* aiBone = mesh->mBones[i];
            Bone bone;
            bone.name = aiBone->mName.C_Str();

            // Récupération de l'indice du parent (si le parent existe)
            aiNode* boneNode = scene->mRootNode->FindNode(aiBone->mName);
            if (boneNode) {
                aiNode* parentNode = boneNode->mParent;
                bone.parentIndex = -1;
                for (unsigned int j = 0; j < mesh->mNumBones; j++) {
                    if (parentNode && mesh->mBones[j]->mName == parentNode->mName) {
                        bone.parentIndex = j;
                        break;
                    }
                }
            }

            // Conversion de la matrice d'offset
            aiMatrix4x4 aiOffsetMatrix = aiBone->mOffsetMatrix;
            bone.offsetMatrix = glm::mat4(
                aiOffsetMatrix.a1, aiOffsetMatrix.b1, aiOffsetMatrix.c1, aiOffsetMatrix.d1,
                aiOffsetMatrix.a2, aiOffsetMatrix.b2, aiOffsetMatrix.c2, aiOffsetMatrix.d2,
                aiOffsetMatrix.a3, aiOffsetMatrix.b3, aiOffsetMatrix.c3, aiOffsetMatrix.d3,
                aiOffsetMatrix.a4, aiOffsetMatrix.b4, aiOffsetMatrix.c4, aiOffsetMatrix.d4
            );

            bones.push_back(bone);
        }
    } else {
        std::cerr << "Erreur : Aucun os trouvé dans le fichier " << filePath << std::endl;
    }

    std::cout << "Chargement des os depuis " << filePath << " réussi. Nombre d'os : " << bones.size() << std::endl;
}
