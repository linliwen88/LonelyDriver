#pragma once

#ifndef __INCLUDE_PHYSX__
#define __INCLUDE_PHYSX__
#include "PxPhysicsAPI.h"
#endif

#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
// #include "./snippetcommon/SnippetPVD.h"

//using namespace physx;
//using namespace physx::vehicle2;
//using namespace snippetvehicle2;

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include <unordered_map>

class Light;
class Drawable;

class Physics
{
public:
	static int  Init();
	static void AddActor(const physx::PxGeometryType::Enum& geoType, Drawable* object);
	static void CreateGround();
	static void initMaterialFrictionTable();
	static int  initVehicles(std::string& gVehicleName);
	static void Step(float deltaTime, std::unordered_map<std::string, glm::mat4>& objectGlobalPoses, Light* light);
	static void stepVehicles();
	static void CleanUp();

private:
	Physics() {}
	Physics(const Physics&) = delete;      // delete copy constructor
	Physics& operator=(Physics&) = delete; // delete assignment operator

	static physx::PxScene*					gScene;
	static physx::PxDefaultAllocator		gAllocator;
	static physx::PxDefaultErrorCallback	gErrorCallback;
	static physx::PxFoundation*				gFoundation;
	static physx::PxPhysics*				gPhysics;
	static physx::PxDefaultCpuDispatcher*	gDispatcher;
	static physx::PxMaterial*				gMaterial;
	static physx::PxPvd*					gPvd;
	static snippetvehicle2::EngineDriveVehicle gVehicle;

	//Gravitational acceleration
	static physx::PxVec3 gGravity;

	// --------------------------
	// vehicle snippet variables
	// --------------------------
	//The vehicle with engine drivetrain
	// static snippetvehicle2::EngineDriveVehicle gVehicle;

	//Vehicle simulation needs a simulation context
	//to store global parameters of the simulation such as 
	//gravitational acceleration.
	static physx::vehicle2::PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	//The mapping between PxMaterial and friction.
	static physx::vehicle2::PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	static physx::PxU32 gNbPhysXMaterialFrictions;
	static physx::PxReal gPhysXDefaultMaterialFriction;
};
