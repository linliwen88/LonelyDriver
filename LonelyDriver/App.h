#pragma once
#include <glm/glm.hpp>
class Window;
class Camera;
class Shader;

class App
{
public:
	App();
	~App();

	void Run();

private:
	int InitOpenGL();
	void UpdateDeltaTime();

	Window* window;

	// camera
	Camera* camera;

	// lighting
	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);

	unsigned int VAO;
	unsigned int VBO;
	unsigned int lightVAO;
	
	Shader* objectShader;
	Shader* lightSourceShader;

	// application informations
	int SCR_WIDTH;
	int SCR_HEIGHT;

	// movement speed
	float deltaTime = 0.0f; // time between current frame and last frame
	float lastFrame = 0.0f; // time of last frame
};
