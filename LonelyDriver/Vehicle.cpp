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

Vehicle::Vehicle(std::string _name, glm::vec3 _position, const char* _path) : 
	Model(_name, glm::vec3(0.0f, -0.875f, 0.0f), _path), // _position set to model class is the offset between the car model and physics vehicle box, not actual position in world space
	Box(_name, _position, 0.84097f, 0.65458f, 2.46971f, false)
{
	Physics::initVehicles(this->Name, glmVec3ToPhysXVec3(_position));
}

Vehicle::~Vehicle()
{
	printf("deleting vehicle\n");
};

void Vehicle::Draw(Shader& shader, glm::mat4 modelMat, bool DrawWireframe, const float wheelDirection)
{
	if (DrawWireframe)
	{
		shader.setMat4("model", modelMat);
		Box::Draw(shader, DrawWireframe);
	}

	Model::Draw(shader, modelMat, wheelDirection);
}