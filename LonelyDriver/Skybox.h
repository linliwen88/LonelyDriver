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
class Shader;

class Skybox : public Drawable
{
public:
	Skybox(std::string _name);
	~Skybox()
	{
		printf("deleting %s vao, vbo, and terxture\n", Name.c_str());
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteTextures(1, &cubemapTexture);
	}

	void Draw(Shader& shader, bool drawWireframe = false) override;

protected:

	void SetUpSkybox();

	unsigned int cubemapTexture;
	unsigned int m_VAO;
	unsigned int m_VBO;
};
