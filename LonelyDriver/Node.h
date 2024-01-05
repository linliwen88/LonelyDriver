#pragma once

#include <vector>

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

class Mesh;
class Shader;

class Node
{
public:
	Node(const char* _name, const glm::mat4& _transformation) : mName(_name), mTransformation(_transformation) {};
	~Node()
	{
		for (int i = 0; i < mChildNodes.size(); i++)
		{
			delete mChildNodes[i];
		}
	}

	std::string mName;

	void Draw(Shader& shader, glm::mat4 modelMat, const int& wheelDirection);
	void AddMesh(Mesh mesh);
	void AddChild(Node* node) { mChildNodes.push_back(node); }

private:
	std::vector<Mesh> mMeshes;
	glm::mat4 mTransformation;
	std::vector<Node*> mChildNodes;
};
