#include "Skybox.hpp"
#include <stb_image.h>

Skybox::Skybox(glm::vec3 pos , float scale):Model(pos )
{ 
    this->setScale(glm::vec3(scale,scale,scale));
}

void Skybox::init(std::string texturePath , std::string filePath )
{
    Model::init(texturePath , filePath);
}
