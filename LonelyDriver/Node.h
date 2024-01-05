#pragma once

#include <vector>
#include <memory>
#include <iostream>

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
	Node(const char* _name);
	~Node();

	std::string mName;
	std::vector<Node*> mChildNodes;
	std::vector<Mesh> mMeshes;

	glm::mat4 mTransformation;

	void Draw(Shader& shader, glm::mat4 modelMat, const int& wheelDirection);
	void AddChild(Node* node) { mChildNodes.push_back(node); }
	void PrintName() {
		std::cout << mName << std::endl;
		for (int i = 0; i < mChildNodes.size(); i++)
		{
			mChildNodes[i]->PrintName();
		}
	}
};
