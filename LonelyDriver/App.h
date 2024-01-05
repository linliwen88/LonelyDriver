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
class Skybox;
class Light;
class Plane;

class App
{
public:
	App(const int width, const int height, const std::string& title);
	~App();

	void Run();

private:
	void UpdateDeltaTimeAndPhysics();
	void StartRender();
	void FinishRender();

	int InitOpenGL();
	void CreateSkybox();
	void CreateTerrain();
	void CreateDrawableObjects();
	void DrawTerrain();

	// application informations
	int SCR_WIDTH;
	int SCR_HEIGHT;
	std::string TITLE;

	// movement speed
	float deltaTime; // time between current frame and last frame
	float lastTime; // time of last frame

	// camera
	Camera* camera;

	// light
	Light* lightCube;
	Shader* lightShader;

	// models
	Model* carModel;
	Shader* modelShader;

	// skybox
	Skybox* skybox;
	Shader* skyboxShader;

	Plane* road;

	// view and projection matrices
	glm::mat4 view;
	glm::mat4 projection;

	std::unordered_map<std::string, glm::mat4> objectGlobalPoses;
	bool DrawWireframe;

	const unsigned int NUM_PATCH_PTS = 4;
	const unsigned rez = 20;
	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainTexture;
	Shader* tessHeightMapShader;

	float oneSec = 0.f;
	int frameCount = 0;

	// physics timing
	// bool physics_DoStep;
	// float physics_TimeCount;
	const float physics_StepTime = 1.0f / 60.0f;
};
