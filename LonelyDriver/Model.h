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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Cube.h";
#include "Mesh.h"
class Shader;

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model : public Cube
{
public:
	Model(std::string _name, glm::vec3 _position, char* path) : Cube(_name, _position, 1.5f, .875f, 3.f), draw_offset(glm::vec3(0.0f, -0.875f, 0.0f))
	{
		LoadModel(path);
	}

	~Model() {};

	// void Draw(Shader& shader, bool DrawWireframe);
	void Draw(Shader& shader, glm::mat4 model, bool DrawWireframe);
private:
	int texture_processed = 0;
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	glm::vec3 draw_offset; // the offset to translate the origin of model to match physX bounding box. Draw wireframe to see effect 

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
