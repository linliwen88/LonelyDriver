#pragma once
#include <glm/glm.hpp>
#include <string>
class Camera;
class Shader;

class App
{
public:
	App(const int width, const int height, const std::string& title);
	~App();

	void Run();

private:
	int InitOpenGL();
	void UpdateDeltaTime();

	// application informations
	int SCR_WIDTH;
	int SCR_HEIGHT;
	std::string TITLE;

	// movement speed
	float deltaTime = 0.0f; // time between current frame and last frame
	float lastFrame = 0.0f; // time of last frame

	// camera
	Camera* camera;

	// lighting
	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	
	Shader* objectShader;
	Shader* lightSourceShader;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int lightVAO;
};
