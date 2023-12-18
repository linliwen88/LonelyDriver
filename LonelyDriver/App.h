#pragma once
#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif
#include <string>
class Camera;
class Shader;
class Model;

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
	float deltaTime; // time between current frame and last frame
	float lastFrame; // time of last frame

	// camera
	Camera* camera;

	// lighting
	glm::vec3 lightPosition;
	
	Shader* objectShader;
	Shader* lightSourceShader;
	Shader* modelShader;

	Model* carModel;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int lightVAO;
};
