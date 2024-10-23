#include "HeightmapTerrain.hpp"
#include <stb_image.h>
#include <iostream>

HeightmapTerrain::HeightmapTerrain(const std::string& heightmapPath , const std::string& texturePath , float scale, float maxHeight)
    : scale(scale), maxHeight(maxHeight), VAO(0), VBO(0), EBO(0) {
    loadHeightmap(heightmapPath);
    generateTerrainMesh();
    init();
    loadTexture(texturePath);
}

HeightmapTerrain::~HeightmapTerrain() {
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
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float heightValue = getHeightAt(x, z);

            // Vertex positions
            vertices.push_back(x * scale);    // x position
            vertices.push_back(heightValue);  // y position (height)
            vertices.push_back(z * scale);    // z position

            // Correct texture coordinates
            float u = static_cast<float>(x) / (width - 1);
            float v = static_cast<float>(z) / (height - 1);
            vertices.push_back(v/3.0f);  // v-coordinate
            vertices.push_back(u/3.0f);  // u-coordinate
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
    /*for (int i = 3 ; i < vertices.size() ; i+=5)
        if(vertices[i] < 0.0f || vertices[i] > 1.0f)
            std::cout << vertices[i] << std::endl;*/
    glUseProgram(shaderProgram);

    if(texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);
    }

    glm::mat4 model = glm::mat4(1.0f); // Identity matrix for model
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

