#include "Node.h"
#include "Mesh.h"

void Node::Draw(Shader& shader, glm::mat4 modelMat, const int& wheelDirection)
{
	modelMat = modelMat * mTransformation;
	
	// rotate wheel
	if (mName.find("front_wheel") != std::string::npos)
	{
		if (wheelDirection == -1) modelMat = glm::rotate(modelMat, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		else if (wheelDirection == 1) modelMat = glm::rotate(modelMat, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	shader.setMat4("model", modelMat);

	// draw meshes of this node
	for (int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i].Draw(shader);
	}

	// draw child nodes
	for (int i = 0; i < mChildNodes.size(); i++)
	{
		mChildNodes[i]->Draw(shader, modelMat, wheelDirection);
	}
}

void Node::AddMesh(Mesh mesh)
{
	mMeshes.push_back(mesh);
}
