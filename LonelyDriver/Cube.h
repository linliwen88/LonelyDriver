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

class Cube : public Drawable
{
public:
	Cube(std::string _name, glm::vec3 _position, float _hLength = 0.5f);
	Cube(std::string _name, glm::vec3 _position, float hx, float hy, float hz);

	~Cube()
	{
		printf("deleting %s vao, vbo, and ebo\n", Name.c_str());
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
	}

    void Draw(Shader& shader, bool drawWireframe) override;

	glm::vec3 m_HalfLength;
	
protected:

	void SetUpCube(float hx, float hy, float hz);
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;
};