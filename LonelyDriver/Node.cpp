#include "Node.h"
#include "Mesh.h"
Node::Node(const char* _name) : mName(_name)
{

}

Node::~Node()
{
	for (int i = 0; i < mChildNodes.size(); i++)
	{
		delete mChildNodes[i];
	}
}

void Node::Draw(Shader& shader, glm::mat4& modelMat, const int& wheelDirection)
{
	// draw meshes of this node
	int size = mMeshes.size();
	modelMat = modelMat * mTransformation;

	for (int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i].Draw(shader, modelMat, wheelDirection);
	}

	// draw child nodes
	for (int i = 0; i < mChildNodes.size(); i++)
	{
		mChildNodes[i]->Draw(shader, modelMat, wheelDirection);
	}
}
