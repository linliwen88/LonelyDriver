#include "Box.h"
#include "Shader.h"
#include "Physics.h"

/// <summary>
/// Create drawable cube object by a half-length of edge  
/// </summary>
/// <param name="_name">name of this drawable object</param>
/// <param name="_position">position of this object in world space</param>
/// <param name="_hLength">helf-length of the cube</param>
/// <param name="_enablePhysics">[true] add this object into physics scene [false] will not add into physics scene</param>
Box::Box(std::string _name, glm::vec3 _position, float _hLength, bool _enablePhysics) : Drawable(_name, _position),
                                                m_HalfLength(glm::vec3(_hLength)),
                                                m_VAO(-1), m_VBO(-1), m_EBO(-1)
{
    SetUpBox(_hLength, _hLength, _hLength);
    if(_enablePhysics) Physics::AddActor(physx::PxGeometryType::eBOX, this);
}

/// <summary>
/// Create drawable box by half-lengths of x, y, z edges 
/// </summary>
/// <param name="_name">name of this drawable object</param>
/// <param name="_position">position of this object in world space</param>
/// <param name="_hLength">helf-length of the cube</param>
/// <param name="_enablePhysics">[true] add this object into physics scene [false] will not add into physics scene</param>
Box::Box(std::string _name, glm::vec3 _position, float _hx, float _hy, float _hz, bool _enablePhysics) : Drawable(_name, _position),
                                                                 m_HalfLength(glm::vec3(_hx, _hy, _hz)),
                                                                 m_VAO(-1), m_VBO(-1), m_EBO(-1)
{
	SetUpBox(_hx, _hy, _hz);
    if (_enablePhysics) Physics::AddActor(physx::PxGeometryType::eBOX, this);
}

void Box::Draw(Shader& shader, bool drawWireframe)
{
    // set polygon mode to draw wireframe
    if (drawWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Box::SetUpBox(float hx, float hy, float hz)
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
