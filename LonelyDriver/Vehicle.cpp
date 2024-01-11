#include "Vehicle.h"
#include "Node.h"

#ifndef __INCLUDE_CONVERTER__
#define __INCLUDE_CONVERTER__
#include "Converter.h"
#endif

#ifndef __INCLUDE_STB_IMAGE__
#define __INCLUDE_STB_IMAGE__
#include "stb_image.h"
#endif

Vehicle::Vehicle(std::string _name, glm::vec3 _position, const char* _path) : Model(_name, _position, _path), Box(_name, _position, 0.84097f, 0.65458f, 2.46971f, false)
{
	Physics::initVehicles(this->Name, glmVec3ToPhysXVec3(_position));
}

Vehicle::~Vehicle()
{
	printf("deleting vehicle\n");
};

void Vehicle::Draw(Shader& shader, glm::mat4 modelMat, bool DrawWireframe, const int& wheelDirection)
{
	if (DrawWireframe)
	{
		shader.setMat4("model", modelMat);
		Box::Draw(shader, DrawWireframe);
	}

	Model::Draw(shader, modelMat, wheelDirection);
}