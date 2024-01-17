#include "Node.h"
#include "Mesh.h"

void Node::Draw(Shader& shader, glm::mat4 modelMat, const float wheelDirection, const int instanceCount)
{
	modelMat = modelMat * mTransformation;

	// rotate wheel
	if (mName.find("front_wheel") != std::string::npos)
	{
		if (wheelDirection > 0.f) modelMat = glm::rotate(modelMat, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		else if (wheelDirection < 0.f) modelMat = glm::rotate(modelMat, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	shader.setMat4("model", modelMat);

	// draw meshes of this node
	for (int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i].Draw(shader, instanceCount);
	}

	// draw child nodes
	for (int i = 0; i < mChildNodes.size(); i++)
	{
		mChildNodes[i]->Draw(shader, modelMat, wheelDirection, instanceCount);
	}
}

void Node::AddMesh(Mesh mesh)
{
	mMeshes.push_back(mesh);
}
