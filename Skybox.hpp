#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Model.hpp"

class Skybox : public Model
{
    public:
        Skybox(glm::vec3 pos , float scale);

        virtual void init(std::string texturePath , std::string filePath = "meshes/cube.glb");
    private:
};

#endif
