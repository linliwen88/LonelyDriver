#pragma once
#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include <unordered_map>
class Camera;
class Shader;
class Model;
class Cube;
class Light;
class Plane;

class App
{
public:
	App(const int width, const int height, const std::string& title);
	~App();

	void Run();

private:
	void StartRender();
	void FinishRender();

	int InitOpenGL();
	void LoadShaders();
	void CreateDrawableObjects();
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

	// light
	Light* lightCube;
	Shader* lightShader;

	// models
	Model* carModel;
	Shader* modelShader;

	Plane* road;

	// view and projection matrices
	glm::mat4 view;
	glm::mat4 projection;

	std::unordered_map<std::string, glm::mat4> objectGlobalPoses;
};
