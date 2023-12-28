#include "Light.h"

Light::Light(std::string _name, glm::vec3 _position, glm::vec3 _color, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	Cube(_name, _position, 5),
	Position(_position), Color(_color),
	Ambient(_ambient), Diffuse(_diffuse), Specular(_specular)
{ }

Light::Light(std::string _name, glm::vec3 _position) : Cube(_name, _position, 0.5f),
								  Position(glm::vec3(2.2f, 2.0f, 2.0f)), Color(glm::vec3(1.0f, 0.0f, 0.0f)), 
								  Ambient(glm::vec3(1.0f, 1.0f, 1.0f)), Diffuse(glm::vec3(0.5f, 0.5f, 0.5f)), Specular(glm::vec3(1.0f, 1.0f, 1.0f))
{ }
