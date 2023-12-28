#pragma once
#ifndef _INCLUDE_GLAD_
#define _INCLUDE_GLAD_
#include <glad/glad.h>
#endif

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include "Drawable.h"

class Plane : public Drawable
{
public:
	Plane(std::string _name);
	~Plane() {}

	void Draw(Shader& shader, bool drawWireframe) override;

private:
	void SetUpPlane();

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;
};
