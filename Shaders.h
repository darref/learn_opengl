#ifndef SHADERS_H
#define SHADERS_H

#include <string>

std::string readShaderFile(const std::string& filePath);
void checkShaderCompilation(unsigned int shaderID);
unsigned int createShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

#endif

