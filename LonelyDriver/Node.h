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

#include <iostream>

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

	void Draw(Shader& shader, glm::mat4 modelMat, const float wheelDirection, const int instanceCount);
	void AddMesh(Mesh mesh);
	void AddChild(Node* node) { mChildNodes.push_back(node); }

private:
	std::vector<Mesh> mMeshes; // The meshes of this node
	glm::mat4 mTransformation; // The transformation relative to the node's parent
	std::vector<Node*> mChildNodes; // The child nodes of this node
};
