#include "HeightmapTerrain.hpp"
#include <stb_image.h>
#include <iostream>

HeightmapTerrain::HeightmapTerrain(const std::string& heightmapPath , const std::string& texturePath , float scale, float maxHeight)
    : scale(scale), maxHeight(maxHeight), VAO(0), VBO(0), EBO(0) 
{
    loadHeightmap(heightmapPath);
    generateTerrainMesh();
    init();
    loadTexture(texturePath);
    position -= glm::vec3(width/2.0f , 0.0f , height/2.0f);
}

HeightmapTerrain::~HeightmapTerrain() 
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void HeightmapTerrain::loadHeightmap(const std::string& heightmapPath) {
    int channels;
    unsigned char* data = stbi_load(heightmapPath.c_str(), &width, &height, &channels, STBI_grey);
    if (!data) {
        std::cerr << "Failed to load heightmap: " << heightmapPath << std::endl;
        return;
    }

    // Resize the heightmapData to match the width and height of the image
    heightmapData.resize(height, std::vector<float>(width, 0.0f));

    // Process each pixel and store the height value
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Convert the grayscale value to a height (0.0 to 1.0)
            float heightValue = static_cast<float>(data[y * width + x]) / 255.0f;
            heightmapData[y][x] = heightValue * maxHeight;  // Scale to maxHeight
            //if(heightValue == 0.0f)
              //  std::cout <<  heightValue << std::endl;       
        }
    }

    stbi_image_free(data);
}


float HeightmapTerrain::getHeightAt(int x, int y) {
    // Ensure x and y are within bounds
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0.0f;  // Return 0 if out of bounds
    }

    // Return the scaled height value from the heightmap data
    return heightmapData[y][x] * scale;
}

void HeightmapTerrain::generateTerrainMesh() {
    //generate vertices and TexCoords
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            // Vertex positions
            vertices.push_back(x * scale);    // x position
            vertices.push_back(getHeightAt(x, z));  // y position (height)
            //if(getHeightAt(x, z) <2.0f)
            //std::cout << getHeightAt(x, z) << std::endl;
            vertices.push_back(z * scale);    // z position

            // Correct texture coordinates
            float u = static_cast<float>(x) / (width - 1);
            float v = static_cast<float>(z) / (height - 1);
            vertices.push_back(v * uniformTexScale);  // v-coordinate
            vertices.push_back(u * uniformTexScale);  // u-coordinate
        }
    }

    // Generate indices for drawing triangles
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = z * width + x;
            int bottomLeft = (z + 1) * width + x;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topLeft + 1);

            // Second triangle
            indices.push_back(bottomLeft);
            indices.push_back(bottomLeft + 1);
            indices.push_back(topLeft + 1);
        }
    }
}

void HeightmapTerrain::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // VBO setup
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // EBO setup
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void HeightmapTerrain::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) 
{
    frustum.setViewProjectionMatrix( projection * view );
    //dessin
    glm::vec3 minT = this->getBoundingBoxMin();
    glm::vec3 maxT = this->getBoundingBoxMax();
    if(!frustum.isBoxInFrustum(minT, maxT))
        return;
    /*for (int i = 3 ; i < vertices.size() ; i+=5)
        if(vertices[i] < 0.0f || vertices[i] > 1.0f)
            std::cout << vertices[i] << std::endl;*/
    glUseProgram(shaderProgram);

    if(texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);
    }

    glm::mat4 model = glm::mat4(1.0f) ; // Identity matrix for model
    model = glm::translate(model, position  );
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);  // Bind VAO to draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }
}

void HeightmapTerrain::loadTexture(const std::string& texturePath) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping and filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Load the texture
    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);  // Flip the texture if needed
    unsigned char* data = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
    }

    stbi_image_free(data);
}

float HeightmapTerrain::calcMinHeight()
{   
    float minHeight = 1000000000.0f;
    for(int i = 1 ; i < vertices.size() ; i+=5)
    {
            if(vertices[i] < minHeight)
                minHeight = vertices[i];       
    }
    std::cout << " POINT LE PLUS BAS DU TERRAIN: " << minHeight << std::endl;
    return minHeight;
}
float HeightmapTerrain::calcMaxHeight()
{   
    float maxHeight = -1000000000.0f;
    for(int i = 1 ; i < vertices.size() ; i+=5)
    {
            if(vertices[i] > maxHeight)
                maxHeight = vertices[i];       
    }
    std::cout << " POINT LE PLUS HAUT DU TERRAIN: " << maxHeight << std::endl;
    return maxHeight;
}

float HeightmapTerrain::calcTerrainHeight()
{   
    float h = maxHeightCurrent - minHeightCurrent;
    //std::cout << " HAUTEUR TOTALE DU TERRAIN: " << h << std::endl;
    return h;
}

void HeightmapTerrain::recalculateBounds() {
    // Initialisation des bornes
    boundsMin = glm::vec3(std::numeric_limits<float>::max());
    boundsMax = glm::vec3(std::numeric_limits<float>::lowest());

    // Calcul des bornes en fonction des vertices et des dimensions
    for (size_t i = 0; i < vertices.size(); i += 5) { // Itérer sur les positions (x, z) et hauteurs (y)
        float x = vertices[i];       // Position x
        float y = vertices[i + 1];   // Hauteur y
        float z = vertices[i + 2];   // Position z

        // Mettre à jour les valeurs minimales et maximales
        boundsMin.x = std::min(boundsMin.x, x);
        boundsMax.x = std::max(boundsMax.x, x);
        boundsMin.y = std::min(boundsMin.y, y);
        boundsMax.y = std::max(boundsMax.y, y);
        boundsMin.z = std::min(boundsMin.z, z);
        boundsMax.z = std::max(boundsMax.z, z);
    }

    // Ajustement des bornes pour inclure la longueur et la largeur
    boundsMin.x = std::min(boundsMin.x, position.x);
    boundsMax.x = std::max(boundsMax.x, position.x + (width * scale));
    boundsMin.z = std::min(boundsMin.z, position.z);
    boundsMax.z = std::max(boundsMax.z, position.z + (height * scale));

    std::cout << "Terrain Bounds Min: (" << boundsMin.x << ", " << boundsMin.y << ", " << boundsMin.z << ")" << std::endl;
    std::cout << "Terrain Bounds Max: (" << boundsMax.x << ", " << boundsMax.y << ", " << boundsMax.z << ")" << std::endl;

    
}

const glm::vec3& HeightmapTerrain::getBoundingBoxMin() const {
    return boundsMin;
}

const glm::vec3& HeightmapTerrain::getBoundingBoxMax() const {
    return boundsMax;
}
