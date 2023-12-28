#include "Plane.h"

Plane::Plane(std::string _name) : Drawable(_name), m_VAO(-1), m_VBO(-1), m_EBO(-1)
{
	SetUpPlane();
}

void Plane::Draw(Shader& shader, bool drawWireframe)
{
	// set polygon mode to draw wireframe
	if (drawWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(m_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Plane::SetUpPlane()
{
	float planeVertices[] = {
		// position			// normal vectors
		-0.5,  0.0, -0.5,	0.0, 1.0, 0.0,		// lower left
		 0.5,  0.0, -0.5,   0.0, 1.0, 0.0,		// lower right
		-0.5,  0.0,  0.5,   0.0, 1.0, 0.0,		// upper left
		 0.5,  0.0,  0.5,   0.0, 1.0, 0.0		// upper right
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO); // generating buffer ID
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_EBO); // generating element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);                   // positions
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // normal vectors
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
