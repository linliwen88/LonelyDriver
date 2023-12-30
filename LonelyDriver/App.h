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
	void StartRender();
	void FinishRender();

	int InitOpenGL();
	void CreateSkybox();
	void CreateTerrain();
	void CreateDrawableObjects();

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

	// skybox
	Skybox* skybox;
	Shader* skyboxShader;

	Plane* road;

	// view and projection matrices
	glm::mat4 view;
	glm::mat4 projection;

	std::unordered_map<std::string, glm::mat4> objectGlobalPoses;
	bool DrawWireframe;

	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainEBO;
	unsigned int NUM_STRIPS;
	unsigned int NUM_VERTS_PER_STRIP;

	float oneSec = 0.f;
	int frameCount = 0;
};
