#pragma once
#include <vector>

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#define __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#include <glm/gtc/matrix_transform.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include "Model.h"
#include "Physics.h"

class Shader;
class Node;

class Vehicle : public Model
{
public:
	Vehicle(std::string _name, glm::vec3 _position, const char* _path) : Model(_name, _position, _path)
	{
		Physics::initVehicles(this->Name);
	};

	~Vehicle();
	
	void Draw(Shader& shader, glm::mat4 modelMat, bool DrawWireframe, const int& carDirection);
};
