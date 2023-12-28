#include "Cube.h"
#include "Shader.h"
#include "Physics.h"

// Create cube by half-height 
Cube::Cube(std::string _name, glm::vec3 _position, float _hLength) : Drawable(_name, _position),
                                                m_HalfLength(glm::vec3(_hLength)),
                                                m_VAO(-1), m_VBO(-1), m_EBO(-1)
{
    SetUpCube(_hLength, _hLength, _hLength);
    Physics::AddActor(physx::PxGeometryType::eBOX, this);
}

// Create box by half-lengths of x, y, z edges 
Cube::Cube(std::string _name, glm::vec3 _position, float _hx, float _hy, float _hz) : Drawable(_name, _position),
                                                                 m_HalfLength(glm::vec3(_hx, _hy, _hz)),
                                                                 m_VAO(-1), m_VBO(-1), m_EBO(-1)
{
	SetUpCube(_hx, _hy, _hz);
    Physics::AddActor(physx::PxGeometryType::eBOX, this);
}

void Cube::Draw(Shader& shader, bool drawWireframe)
{
    // set polygon mode to draw wireframe
    if (drawWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Cube::SetUpCube(float hx, float hy, float hz)
{
    // hx, hy, hz are half lengths of x, y, z edges

    float cubeVertices[] = {
        // position           // normal vectors
        -hx, -hy, -hz,  0.0f,  0.0f, -1.0f,
         hx, -hy, -hz,  0.0f,  0.0f, -1.0f,
         hx,  hy, -hz,  0.0f,  0.0f, -1.0f,
         hx,  hy, -hz,  0.0f,  0.0f, -1.0f,
        -hx,  hy, -hz,  0.0f,  0.0f, -1.0f,
        -hx, -hy, -hz,  0.0f,  0.0f, -1.0f,

        -hx, -hy,  hz,  0.0f,  0.0f, 1.0f,
         hx, -hy,  hz,  0.0f,  0.0f, 1.0f,
         hx,  hy,  hz,  0.0f,  0.0f, 1.0f,
         hx,  hy,  hz,  0.0f,  0.0f, 1.0f,
        -hx,  hy,  hz,  0.0f,  0.0f, 1.0f,
        -hx, -hy,  hz,  0.0f,  0.0f, 1.0f,

        -hx,  hy,  hz, -1.0f,  0.0f,  0.0f,
        -hx,  hy, -hz, -1.0f,  0.0f,  0.0f,
        -hx, -hy, -hz, -1.0f,  0.0f,  0.0f,
        -hx, -hy, -hz, -1.0f,  0.0f,  0.0f,
        -hx, -hy,  hz, -1.0f,  0.0f,  0.0f,
        -hx,  hy,  hz, -1.0f,  0.0f,  0.0f,

         hx,  hy,  hz,  1.0f,  0.0f,  0.0f,
         hx,  hy, -hz,  1.0f,  0.0f,  0.0f,
         hx, -hy, -hz,  1.0f,  0.0f,  0.0f,
         hx, -hy, -hz,  1.0f,  0.0f,  0.0f,
         hx, -hy,  hz,  1.0f,  0.0f,  0.0f,
         hx,  hy,  hz,  1.0f,  0.0f,  0.0f,

        -hx, -hy, -hz,  0.0f, -1.0f,  0.0f,
         hx, -hy, -hz,  0.0f, -1.0f,  0.0f,
         hx, -hy,  hz,  0.0f, -1.0f,  0.0f,
         hx, -hy,  hz,  0.0f, -1.0f,  0.0f,
        -hx, -hy,  hz,  0.0f, -1.0f,  0.0f,
        -hx, -hy, -hz,  0.0f, -1.0f,  0.0f,

        -hx,  hy, -hz,  0.0f,  1.0f,  0.0f,
         hx,  hy, -hz,  0.0f,  1.0f,  0.0f,
         hx,  hy,  hz,  0.0f,  1.0f,  0.0f,
         hx,  hy,  hz,  0.0f,  1.0f,  0.0f,
        -hx,  hy,  hz,  0.0f,  1.0f,  0.0f,
        -hx,  hy, -hz,  0.0f,  1.0f,  0.0f
    };

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO); // generating buffer ID
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);                   // positions
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // normal vectors
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
