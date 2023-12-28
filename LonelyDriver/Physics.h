#pragma once

#ifndef __INCLUDE_PHYSX__
#define __INCLUDE_PHYSX__
#include "PxPhysicsAPI.h"
#endif

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#include <string>
#include <unordered_map>


class Physics
{
public:
	static void Init();
	static void AddActor();
	static void Step(std::unordered_map<std::string, glm::mat4>& objectGlobalPoses);
	static void CleanUp();
	static physx::PxScene* gScene;

private:
	Physics() {}
	Physics(const Physics&) = delete;      // delete copy constructor
	Physics& operator=(Physics&) = delete; // delete assignment operator

	static physx::PxDefaultAllocator		gAllocator;
	static physx::PxDefaultErrorCallback	gErrorCallback;
	static physx::PxFoundation* gFoundation;
	static physx::PxPhysics* gPhysics;
	static physx::PxDefaultCpuDispatcher* gDispatcher;
	static physx::PxMaterial* gMaterial;
	static physx::PxPvd* gPvd;
};
