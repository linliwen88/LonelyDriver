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

#include "Box.h"
#include "Model.h"
#include "Physics.h"

class Node;
class Shader;

class Vehicle : public Model, public Box
{
public:
	Vehicle(std::string _name, glm::vec3 _position, const char* _path);

	~Vehicle();

	glm::vec3& GetPosition()
	{
		return position;
	}

	glm::vec4& GetRotation()
	{
		return rotation;
	}
	void Draw(Shader& shader, glm::mat4 modelMat, bool DrawWireframe, const int& carDirection);

private:
	glm::vec3 position;
	glm::vec4 rotation; // (x, y, z, deg), rotation represented with an axis of 3 scalars and an angle expressed in degrees.
};
