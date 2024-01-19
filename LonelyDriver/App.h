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
class Vehicle;
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
	void RenderScene(Shader* shader = nullptr);
	void FinishRender();
	void RenderDepthMap();
	void ShowDepthMap();

	int  InitOpenGL();
	void CreateSkybox();
	void CreateTerrain();
	void CreateDrawableObjects();
	void DrawTerrain(Shader* shader);

	void InitShadowMapping();
	void GenerateFrameBuffer();

	// application informations
	int SCR_WIDTH;
	int SCR_HEIGHT;
	std::string TITLE;

	// movement speed
	float deltaTime; // time between current frame and last frame
	float lastTime; // time of last frame
	float physicsTimeCounter;

	// camera
	Camera* camera;

	// light
	Light* lightSource;
	Shader* lightShader; // unused

	// models
	Model* tree;
	Vehicle* carModel;
	Shader* modelShader;
	Shader* modelTreeShader;


	// skybox
	Skybox* skybox;
	Shader* skyboxShader;

	Plane* road;

	// display depth map
	Plane* canvas;
	Shader* displayDepthMapShader;

	// view and projection matrices
	glm::mat4 viewSkybox; // special view matrix for rendering skybox. Rremove translation section, only keep rotation section of camera
	glm::mat4 view;
	glm::mat4 projection;

	std::unordered_map<std::string, glm::mat4> objectGlobalPoses;
	bool DrawWireframe;
	
	// framebuffer for rendering depth map
	Shader* depthShader;
	unsigned int depthMapFBO;
	unsigned int depthMapTexture;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glm::mat4 lightSpaceMatrix;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	float leftPlane = -200.f;
	float rightPlane = 200.f;
	float topPlane = 200.f;
	float bottomPlane = -200.f;

	// variables for terrain
	const unsigned int NUM_PATCH_PTS = 4;
	const unsigned rez = 20;
	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainHeightTexture;
	unsigned int terrainColorTexture;
	Shader* tessShader;
	int terrainTexWidth, terrainTexHeight;

	float oneSec = 0.f;
	int frameCount = 0;

	// physics timing
	// bool physics_DoStep;
	// float physics_TimeCount;
	const float physicsStepTime = 1.0f / 60.0f;
};
