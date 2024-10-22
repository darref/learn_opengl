#include "Model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <math.h>


Model::Model()
    : texture(0),         // Initialize texture first
      VAO(0),             // Then initialize VAO
      VBO(0),             // Then initialize VBO
      EBO(0),             // Then initialize EBO
      position(glm::vec3(0.0f, 0.0f, 0.0f)),  // Initialize position
      rotation(glm::vec3(0.0f, 0.0f, 0.0f)),  // Initialize rotation
      scale(glm::vec3(1.0f, 1.0f, 1.0f)),     // Initialize scale
      matrice(glm::mat4(1.0f))                 // Initialize matrice
{
}

Model::Model(glm::vec3 pos)
    : texture(0),         // Initialize texture first
      VAO(0),             // Then initialize VAO
      VBO(0),             // Then initialize VBO
      EBO(0),             // Then initialize EBO
      position(pos),      // Use the provided position
      rotation(glm::vec3(0.0f, 0.0f, 0.0f)),  // Initialize rotation
      scale(glm::vec3(1.0f, 1.0f, 1.0f)),     // Initialize scale
      matrice(glm::mat4(1.0f))                 // Initialize matrice
{
}


void Model::init(std::string texturePath , std::string filePath) {
    loadVertices(filePath);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Génération de l'EBO

    glBindVertexArray(VAO);
    
    // Liez le VBO et envoyez les sommets
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Liez l'EBO et envoyez les indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Coordonnées de texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Chargement de la texture si le chemin est fourni
    if (!texturePath.empty()) {
        loadTexture(texturePath);
    }

    glBindVertexArray(0); // Désactivez le VAO pour éviter les modifications accidentelles
}


void Model::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    applyTransformations();

    glUseProgram(shaderProgram);

    if(!(texture == 0))
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

    }

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(matrice));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Exemple de rendu
    glBindVertexArray(VAO); // Liez votre VAO ici
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Utilisez glDrawElements pour dessiner les faces
    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }
}


void Model::resetMatrice()
{
    matrice = glm::mat4(1.0f);
}

void Model::setPosition(glm::vec3 pos)
{
    position = pos;
}

void Model::setRotation(glm::vec3 rot)
{
    rotation = rot;
    if(rotation.x > 360)
        rotation.x = rotation.x - 360;
    else if (rotation.x < 0)
        rotation.x = 360 - abs(rotation.x);

    if(rotation.y > 360)
        rotation.y = rotation.y - 360;
    else if (rotation.y < 0)
        rotation.y = 360 - abs(rotation.y);


    if(rotation.z > 360)
        rotation.z = rotation.z - 360;
    else if (rotation.z < 0)
        rotation.z = 360 - abs(rotation.z);

}

void Model::setScale(glm::vec3 sca)
{
    scale = sca;
}

void Model::setModelMatrix(glm::mat4& matrix)
{
    matrice = matrix;
}

void Model::animation(float deltaTime) {
    if(spinning)
    {
        addRotation(glm::vec3(0.0f,spinningSpeed*deltaTime , 0.0f));
    }
}

// Méthodes pour ajouter à la position, rotation, et échelle
void Model::addPosition(glm::vec3 deltaPos) {
    position += deltaPos; 
}

void Model::addRotation(glm::vec3 deltaRot) {
    rotation += deltaRot;
    
    if(rotation.x > 360)
        rotation.x = rotation.x - 360;
    else if (rotation.x < 0)
        rotation.x = 360 - abs(rotation.x);

    if(rotation.y > 360)
        rotation.y = rotation.y - 360;
    else if (rotation.y < 0)
        rotation.y = 360 - abs(rotation.y);


    if(rotation.z > 360)
        rotation.z = rotation.z - 360;
    else if (rotation.z < 0)
        rotation.z = 360 - abs(rotation.z);
}

void Model::addScale(glm::vec3 deltaScale) {
    scale += deltaScale;
}

const std::vector<float>& Model::getVertices() const
{
    return vertices;
}

const std::vector<unsigned int>& Model::getIndices() const 
{
    return indices;
}

const glm::mat4& Model::getMatrice() const
{
    return matrice;
}

void Model::applyTransformations()
{
    resetMatrice();
    matrice = glm::translate(matrice, position);
    matrice = glm::scale(matrice, scale); // Scale before rotation
    matrice = glm::rotate(matrice, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    matrice = glm::rotate(matrice, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    matrice = glm::rotate(matrice, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Model::loadTexture(const std::string& texturePath) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  

        std::cout << "Texture chargée avec succès depuis " << texturePath << std::endl;
    } else {
        std::cerr << "Échec de chargement de la texture : " << texturePath << std::endl;
    }

    stbi_image_free(data);
}

void Model::loadVertices(const std::string& filePath) {
    Assimp::Importer importer;

    // Charger le fichier avec Assimp
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FlipUVs |
        aiProcess_ConvertToLeftHanded |
        aiProcess_ValidateDataStructure);

    if (scene) {
        std::cout << "Import Successful!" << std::endl;
        std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;

        // Clear previous data
        vertices.clear();
        indices.clear();

        // Parcourir tous les meshes dans la scène
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            
            // Itérer sur les sommets du mesh
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                // Position
                vertices.push_back(mesh->mVertices[j].x);
                vertices.push_back(mesh->mVertices[j].y);
                vertices.push_back(mesh->mVertices[j].z);

                // Coordonnées de texture
                if (mesh->mTextureCoords[0]) {
                    vertices.push_back(mesh->mTextureCoords[0][j].x);
                    vertices.push_back(mesh->mTextureCoords[0][j].y);
                } else {
                    // Coordonnées de texture par défaut si manquantes
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
            }

            // Itérer sur les faces pour obtenir les indices
            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k]);
                }
            }
        }
        std::cout << "Loaded " << scene->mNumMeshes << " meshes and " << vertices.size() / 5 << " vertices from " << filePath << std::endl;
    } else {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
    }

    /*// Affichage des sommets et des indices
    std::cout << "Vertices: " << vertices.size() / 5 << std::endl;
    for (size_t i = 0; i < vertices.size(); i += 5) {
        std::cout << "Position: (" << vertices[i] << ", " << vertices[i + 1] << ", " << vertices[i + 2] << ") "
                  << "TexCoords: (" << vertices[i + 3] << ", " << vertices[i + 4] << ")\n";
    }

    std::cout << "Indices: " << indices.size() << std::endl;
    for (size_t i = 0; i < indices.size(); i++) {
        std::cout << "Index: " << indices[i] << std::endl;
    }*/
}





