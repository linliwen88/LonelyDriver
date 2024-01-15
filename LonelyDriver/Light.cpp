#include "Light.h"

Light::Light(glm::vec3 _position) :
	Position(_position),
	Color(glm::vec3(1.0f, 0.0f, 0.0f)), 
	Ambient(glm::vec3(1.0f, 1.0f, 1.0f)), 
	Diffuse(glm::vec3(0.5f, 0.5f, 0.5f)), 
	Specular(glm::vec3(1.0f, 1.0f, 1.0f))
{ }

Light::Light(glm::vec3 _position, glm::vec3 _color, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	Position(_position),
	Color(_color),
	Ambient(_ambient), 
	Diffuse(_diffuse), 
	Specular(_specular)
{ }

