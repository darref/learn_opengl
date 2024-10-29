#ifndef MODEL_HPP
#define MODEL_HPP

#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <string>

#include "ObjectBase.hpp"
using namespace std;
#include "Frustum.hpp"
#include "Skeleton.hpp"
#include "Animation.hpp"

#define JUMP_ANIM "jump"
#define RUN_ANIM "run"
#define IDLE_ANIM "idle"

class Model: public ObjectBase
{
public:
    Model();
    Model(glm::vec3 pos);

    virtual ~Model()
    {
        // Nettoyage
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    virtual void init(std::string texturePath , std::string filePath);
    ////////////////////////////////////////////////////////
    virtual void animation(float deltaTime);
    void play(float deltaTime);
    void setAnimation(const std::string& animationName);
    const std::vector<float>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;
    const std::vector<float>& getTextureCoords() const;
    const glm::mat4& getMatrice() const;
    const GLuint& getTexture() const;
    const std::string getName() const;
    float getSizeX() const;
    float getSizeY() const;
    float getSizeZ() const;
    glm::vec3 getBoundingBoxMin() const;
    glm::vec3 getBoundingBoxMax() const;
    // Ajouts dans la section public de Model.hpp
    glm::vec3 getRotation() const;
    glm::vec3 getDirection() const;
    const glm::vec3& getPosition() const;
    // Ajoutez ces méthodes dans la section public de Model.hpp
    glm::vec3 getRightVector() const;
    glm::vec3 getUpVector() const;


    void setPosition(glm::vec3 pos);
    void setRotation(glm::vec3 rot);
    void setScale(glm::vec3 sca);
    void setModelMatrix(glm::mat4& matrix);
    void setName(const std::string& name);
    void addPosition(glm::vec3 deltaPos);
    void addRotation(glm::vec3 deltaPos);
    void addScale(glm::vec3 deltaPos);

    void applyTransformations();
    virtual void loadTexture(const std::string& texturePath);
    void loadVertices(const std::string& filePath);
    void loadSkeleton(const string& meshFilePath);
    void loadAnimation(const string& animFilePath  , const string& animationName);
    virtual void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);
    ////////////////////////////////////////////////////////
protected:
    // Order of initialization in the constructor
    GLuint texture;                          // Texture ID
    GLuint VAO, VBO, EBO;                   // OpenGL identifiers for buffers
    glm::vec3 position;                      // Position of the model
    glm::vec3 rotation;                      // Rotation of the model
    glm::vec3 scale;                         // Scale of the model
    glm::mat4 matrice;                       // Transformation matrix
    std::vector<float> vertices;             // Vertex data
    std::vector<unsigned int> indices;       // Index data
    bool spinning  = false;
    float spinningSpeed = 30.0f;
    std::string name = "default name";
    Frustum frustum;
    Skeleton skeleton;
    std::map<std::string, Animation*> animations;
    Animation* currentAnimation;
    std::vector<float> animatedVertices;

    void resetMatrice();                     // Reset transformation matrix
};

#endif

