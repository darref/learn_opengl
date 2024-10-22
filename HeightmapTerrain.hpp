#ifndef HEIGHTMAPTERRAIN_HPP
#define HEIGHTMAPTERRAIN_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>   // Pour les transformations
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

class HeightmapTerrain {
public:
    HeightmapTerrain(const std::string& heightmapPath,const std::string& texturePath , float scale, float maxHeight);
    ~HeightmapTerrain();

    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);
    void loadHeightmap(const std::string& heightmapPath);
    void generateTerrainMesh();
    void init();
    void loadTexture(const std::string& texturePath);

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    int width, height;
    float scale, maxHeight;
    GLuint texture;
    std::vector<std::vector<float>> heightmapData;

    float getHeightAt(int x, int y);
};

#endif
