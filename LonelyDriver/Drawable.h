#pragma once

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

class Shader;

class Drawable
{
public:
	Drawable(std::string _name, glm::vec3 _position = glm::vec3(0.0f)) : Name(_name), Position(_position) {};
	virtual ~Drawable() {}

	virtual void Draw(Shader& shader, bool drawWireframe) = 0;

	std::string Name;
	glm::vec3 Position;
};
