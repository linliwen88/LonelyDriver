#pragma once
#include <vector>

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#define __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#include <glm/gtc/matrix_transform.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#ifndef __INCLUDE_STB_IMAGE__
#define __INCLUDE_STB_IMAGE__
#include "stb_image.h"
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Box.h";
#include "Mesh.h"
class Shader;
class Node;

static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
	Model(std::string _name, glm::vec3 _position, const char* path) : rootNode(nullptr), directory(""), position(_position)
	{
		LoadModel(path);
	}

	~Model();
	
	void Draw(Shader& shader, glm::mat4 modelMat, const float carDirection);
	void Draw(Shader& shader, glm::mat4 modelMat, const float wheelDirection, const int instanceCount);
	
private:
	Node* rootNode;
	std::string directory;
	glm::vec3 position; // world space
	// glm::vec3 draw_offset; // the offset to translate the origin of model to match physX bounding box. Draw wireframe to see effect 
	std::vector<Texture> textures_loaded;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene, Node* currNode);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
