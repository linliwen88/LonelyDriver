#pragma once

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include "Box.h"

class Light : public Box
{
public:
    Light(std::string _name, glm::vec3 _position);

    Light(std::string _name, glm::vec3 _position, glm::vec3 _color,
          glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);

    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
};
