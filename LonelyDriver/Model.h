#pragma once
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Drawable.h";
#include "Mesh.h"
class Shader;

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model : public Drawable
{
public:
	Model(char* path)
	{
		LoadModel(path);
	}

	~Model() {}

	void Draw(Shader& shader) override;
private:
	int texture_processed = 0;
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
