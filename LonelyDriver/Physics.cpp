#include "Physics.h"
#include "Box.h"
#include "Drawable.h"

#ifndef __INCLUDE_CONVERTER__
#define __INCLUDE_CONVERTER__
#include "Converter.h"
#endif

#include <iostream>

#define PVD_HOST "127.0.0.1"

#include <ctype.h>

physx::PxDefaultAllocator	                    Physics::gAllocator;
physx::PxDefaultErrorCallback                   Physics::gErrorCallback;
physx::PxFoundation*                            Physics::gFoundation      = NULL;
physx::PxPhysics*                               Physics::gPhysics         = NULL;
physx::PxDefaultCpuDispatcher*                  Physics::gDispatcher      = NULL;
physx::PxScene*                                 Physics::gScene           = NULL;
physx::PxMaterial*                              Physics::gMaterial        = NULL;
physx::PxPvd*                                   Physics::gPvd             = NULL;
physx::PxVec3                                   Physics::gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
snippetvehicle2::EngineDriveVehicle             Physics::gVehicle;
physx::vehicle2::PxVehiclePhysXSimulationContext Physics::gVehicleSimulationContext;
// physx::PxConvexMesh*                            Physics::gUnitCylinderSweepMesh = NULL;

physx::PxU32                                    Physics::gNbPhysXMaterialFrictions = 0;
physx::PxReal                                   Physics::gPhysXDefaultMaterialFriction = 1.0f;
physx::vehicle2::PxVehiclePhysXMaterialFriction Physics::gPhysXMaterialFrictions[16];
physx::PxRigidStatic* groundPlane = NULL;

// --------------
// Vehicle 
// --------------
const char gVehicleName[] = "EngineDrive";
const char* gVehicleDataPath = ".\\vehicledata";

//Commands are issued to the vehicle in a pre-choreographed sequence.

const physx::PxU32 gTargetGearCommand = physx::vehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
Command gCommands[] =
{
    {0.5f, 0.0f, 0.0f, gTargetGearCommand, 2.0f},	//brake on and come to rest for 2 seconds
    {0.0f, 0.65f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
    {1.0f, 0.0f, 0.0f, gTargetGearCommand, 2.0f},	//brake for 5 seconds
    {0.0f, 0.75f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
    {0.0f, 0.25f, -0.5f, gTargetGearCommand, 2.0f}	//light throttle and steer for 5 seconds.
};
const physx::PxU32 gNbCommands = sizeof(gCommands) / sizeof(Command);
physx::PxReal gCommandTime = 0.0f;			//Time spent on current command
physx::PxU32 gCommandProgress = 0;			//The id of the current command.

Command Physics::CurrentVehicleCommand = { 0.0f, 0.0f, 0.0f, gTargetGearCommand, 5.0 };


int Physics::Init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    // sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.gravity = gGravity;
    gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    
    return true;
}

void Physics::CreateGround(const int& terrainTexWidth, const int& terrainTexHeight, const int& nChannels, unsigned char* data)
{
    //// create simple plane
    //groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1.0, 0, 0), *gMaterial);
    //groundPlane->setName("ground");
    //gScene->addActor(*groundPlane);

    // create heightmap data
    // heightfeild dimensions
    const physx::PxU32 nRows = terrainTexHeight;
    const physx::PxU32 nCols = terrainTexWidth;

    // create the actor for heightfield
    physx::PxRigidStatic* heightfieldActor = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));

    // iterate over source data points and find minimum and maximum heights
    // physx::PxReal minHeight = PX_MAX_F32;
    // physx::PxReal maxHeight = -PX_MAX_F32;
    //for (physx::PxU32 s = 0; s < nRows * nCols; s++)
    //{
    //    physx::PxReal height = data[s * nChannels];
    //    minHeight = physx::PxMin(minHeight, height);
    //    maxHeight = physx::PxMax(maxHeight ,height);
    //}
    physx::PxReal minHeight = (physx::PxReal)0x00;
    physx::PxReal maxHeight = (physx::PxReal)UCHAR_MAX;


    // compute maximum height difference
    physx::PxReal deltaHeight = maxHeight - minHeight;

    // maximum positive value that can be represented with signed 16 bit integer
    physx::PxReal quantization = (physx::PxReal)0x7fff;

    // compute heightScale such that the forward transform will generate the closest point
    // to the source
    // clamp to at least PX_MIN_HEIGHTFIELD_Y_SCALE to respect the PhysX API specs
    physx::PxReal heightScale = physx::PxMax(deltaHeight / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);
    heightScale = heightScale / 3.0f;
    printf("heightScale: %f\n", heightScale);

    physx::PxHeightFieldSample* hfSamples = new physx::PxHeightFieldSample[nRows * nCols];

    physx::PxU32 index = 0;
    for (physx::PxU32 row = 0; row < nRows; row++)
    {
        for (physx::PxU32 col = 0; col < nCols; col++)
        {
            // get average hieght of neighboring samples
            physx::PxReal sample = 0.0f;
            for (int i = -1; i < 2; i++)
            {
                int r = row + i;
                r = (r < 0 || r >= nRows) ? row : r;
                for (int j = -1; j < 2; j++)
                {
                    int c = col + j;
                    c = (c < 0 || c >= nCols) ? col : c;

                    sample += data[((r * nCols) + c) * nChannels];
                }
            }
            sample /= 9.0f;

            physx::PxI16 height;
            // height = physx::PxI16(quantization * ((((physx::PxReal)data[((row * nCols) + col) * nChannels] - minHeight) / deltaHeight) - (10.f / UCHAR_MAX)));
            height = physx::PxI16(quantization * (((sample - minHeight) / deltaHeight) - (10.f / UCHAR_MAX)));

            /*std::cout << "data: " << (physx::PxReal)data[((row * nCols) + col) * nChannels] << std::endl;
            std::cout << "data after scaling: " << ((((physx::PxReal)data[((row * nCols) + col) * nChannels] - minHeight) / deltaHeight) - (10.f / UCHAR_MAX)) << std::endl;
            std::cout << "data after quantization: " << quantization * ((((physx::PxReal)data[((row * nCols) + col) * nChannels] - minHeight) / deltaHeight) - (10.f / UCHAR_MAX)) << std::endl;
            std::cout << "height: " << height << std::endl;*/

            physx::PxHeightFieldSample& smp = hfSamples[(col * nRows) + row];
            smp.height = height;
            smp.materialIndex0 = index;
            smp.materialIndex1 = index;
            //if (userFlipEdge)
            //    smp.setTessFlag();
        }
    }

    // Build PxHeightFieldDesc from samples
    physx::PxHeightFieldDesc terrainDesc;
    terrainDesc.format = physx::PxHeightFieldFormat::eS16_TM;
    terrainDesc.nbColumns = nCols;
    terrainDesc.nbRows = nRows;
    terrainDesc.samples.data = hfSamples;
    terrainDesc.samples.stride = sizeof(physx::PxHeightFieldSample);
    terrainDesc.flags = physx::PxHeightFieldFlags();

    physx::PxHeightFieldGeometry hfGeom;
    // physx::PxReal terrainWidth = 1000;
    hfGeom.columnScale = (float)(terrainTexWidth / 4.0f) / (nCols - 1); // compute column and row scale from input terrain
    // height grid
    hfGeom.rowScale = (float)(terrainTexHeight / 4.0f) / (nRows - 1);
    hfGeom.heightScale = deltaHeight != 0.0f ? heightScale : 1.0f;
    hfGeom.heightField = PxCreateHeightField(terrainDesc, gPhysics->getPhysicsInsertionCallback());

    delete[] hfSamples;

    physx::PxTransform localPose;
    localPose.p = physx::PxVec3(-((float)(terrainTexWidth / 4.0f) * 0.5f),    // make it so that the center of the
        minHeight, -((float)(terrainTexHeight / 4.0f) * 0.5f));         // heightfield is at world (0,minHeight,0)
    localPose.q = physx::PxQuat(physx::PxIdentity);
    physx::PxU16 nbMaterials = 1;
    physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*heightfieldActor, hfGeom, &gMaterial, nbMaterials);
    shape->setLocalPose(localPose);

    heightfieldActor->setName("ground");
    gScene->addActor(*heightfieldActor);
}

void Physics::initMaterialFrictionTable()
{
    //Each physx material can be mapped to a tire friction value on a per tire basis.
    //If a material is encountered that is not mapped to a friction value, the friction value used is the specified default value.
    //In this snippet there is only a single material so there can only be a single mapping between material and friction.
    //In this snippet the same mapping is used by all tires.
    gPhysXMaterialFrictions[0].friction = 1.0f;
    gPhysXMaterialFrictions[0].material = gMaterial;
    gPhysXDefaultMaterialFriction = 1.0f;
    gNbPhysXMaterialFrictions = 1;
}

int Physics::initVehicles(std::string& gVehicleName, physx::PxVec3 position)
{
    physx::vehicle2::PxInitVehicleExtension(*gFoundation);

    initMaterialFrictionTable();

    //Load the params from json or set directly.
    readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", gVehicle.mBaseParams);
    readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json", gVehicle.mEngineDriveParams);

    setPhysXIntegrationParams(gVehicle.mBaseParams.axleDescription, gPhysXMaterialFrictions, 
        gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction, gVehicle.mPhysXParams);

    //Set the states to default.
    if (!gVehicle.initialize(*gPhysics, physx::PxCookingParams(physx::PxTolerancesScale()), *gMaterial, snippetvehicle2::EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE))
    {
        return false;
    }

    //Apply a start pose to the physx actor and add it to the physx scene.
    // physx::PxTransform pose(physx::PxVec3(0.000000000f, -0.500000119f, -1.59399998f), physx::PxQuat(physx::PxIdentity));
    physx::PxTransform pose(position, physx::PxQuat(physx::PxIdentity));
    gVehicle.setUpActor(*gScene, pose, gVehicleName.c_str());

    //Set the vehicle in 1st gear.
    gVehicle.mEngineDriveState.gearboxState.currentGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
    gVehicle.mEngineDriveState.gearboxState.targetGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
    
    //Set the vehicle to use the automatic gearbox.
    gVehicle.mTransmissionCommandState.targetGear = physx::vehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

    //Set up the simulation context.
    //The snippet is set up with
    //a) z as the longitudinal axis
    //b) x as the lateral axis
    //c) y as the vertical axis.
    //d) metres  as the lengthscale.
    gVehicleSimulationContext.setToDefault();
    gVehicleSimulationContext.frame.lngAxis = physx::vehicle2::PxVehicleAxes::ePosZ;
    gVehicleSimulationContext.frame.latAxis = physx::vehicle2::PxVehicleAxes::ePosX;
    gVehicleSimulationContext.frame.vrtAxis = physx::vehicle2::PxVehicleAxes::ePosY;
    gVehicleSimulationContext.scale.scale = 1.0f;
    gVehicleSimulationContext.gravity = gGravity;
    gVehicleSimulationContext.physxScene = gScene;
    gVehicleSimulationContext.physxActorUpdateMode = physx::vehicle2::PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;

    /*gUnitCylinderSweepMesh = PxVehicleUnitCylinderSweepMeshCreate(gVehicleSimulationContext.frame,
        *gPhysics, physx::PxCookingParams(physx::PxTolerancesScale()));
    if (!gUnitCylinderSweepMesh)
    {
        return false;
    }
    gVehicleSimulationContext.physxUnitCylinderSweepMesh = gUnitCylinderSweepMesh;*/

    return true;
}
/// <summary>
/// create actor of 'geotType' for 'object' and put into physX scene
/// </summary>
/// <param name="geoType">specify desired geometric type</param>
/// <param name="object">the the object to put into scene </param>
void Physics::AddActor(const physx::PxGeometryType::Enum& geoType, Drawable* object)
{
    if (geoType == physx::PxGeometryType::eBOX)
    {
        // create a box
        Box* cubeObject = static_cast<Box*>(object);
        
        // physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(cubeObject->hx / 2.f, cubeObject->hy / 2.f, cubeObject->hz / 2.f), *gMaterial);
        physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(cubeObject->m_HalfLength.x, cubeObject->m_HalfLength.y, cubeObject->m_HalfLength.z), *gMaterial);

        physx::PxTransform t = physx::PxTransform(glmVec3ToPhysXVec3(cubeObject->Position));
        physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(t);
        body->attachShape(*shape);
        body->setName(cubeObject->Name.c_str());
        physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
        gScene->addActor(*body);
        shape->release();
    }
}

void Physics::Step(float deltaTime, std::unordered_map<std::string, glm::mat4>& objectGlobalPoses, 
                   glm::vec3& vehiclePosition, glm::vec4& vehicleRotation)
{
    stepVehicles(deltaTime);

    gScene->simulate(deltaTime);
    gScene->fetchResults(true);

    //Increment the time spent on the current command.
    //Move to the next command in the list if enough time has lapsed.
    gCommandTime += deltaTime;
    if (gCommandTime > gCommands[gCommandProgress].duration)
    {
        gCommandProgress++;
        gCommandTime = 0.0f;
    }

    physx::PxU32 nbActors = gScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
    glm::mat4 newModelMat;
    if (nbActors)
    {
        std::vector<physx::PxRigidActor*> actors(nbActors);
        gScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(&actors[0]), nbActors);

        // render actor
        physx::PxShape* shapes[128];
        physx::PxU32 numActors = static_cast<physx::PxU32>(actors.size());
        for (physx::PxU32 i = 0; i < numActors; i++)
        {
            const physx::PxU32 nbShapes = actors[i]->getNbShapes();
            PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
            actors[i]->getShapes(shapes, nbShapes);

            for (physx::PxU32 j = 0; j < nbShapes; j++)
            {
                const physx::PxMat44 shapePose(physx::PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
                const physx::PxGeometry& geom = shapes[j]->getGeometry();
                if (std::string(actors[i]->getName()) == "car" && (geom.getType() != physx::PxGeometryType::eBOX))
                {
                    break;
                }
                
                // printf("actor name: %s shape: %d\n", actors[i]->getName(), geom.getType());
                newModelMat = PhysXMat4ToglmMat4(shapePose);
                objectGlobalPoses[actors[i]->getName()] = newModelMat;

                // get vehicle position and rotation
                if (std::string(actors[i]->getName()) == "car")
                {
                    vehiclePosition = PhysXVec3ToglmVec3(physx::PxShapeExt::getGlobalPose(*shapes[j], *actors[i]).p);

                    float angle;
                    physx::PxVec3 axis;
                    physx::PxShapeExt::getGlobalPose(*shapes[j], *actors[i]).q.toRadiansAndUnitAxis(angle, axis);

                    vehicleRotation = PhysXVec4ToglmVec4(physx::PxVec4(axis, angle));
                }
            }
        }
    }
}

void Physics::stepVehicles(float deltaTime)
{
    //if (gNbCommands == gCommandProgress)
    //    return;

    ////Apply the brake, throttle and steer to the command state of the vehicle.
    //const Command& command = gCommands[gCommandProgress];
    //gVehicle.mCommandState.brakes[0] = command.brake;
    //gVehicle.mCommandState.nbBrakes = 1;
    //gVehicle.mCommandState.throttle = command.throttle;
    //gVehicle.mCommandState.steer = command.steer;
    //gVehicle.mTransmissionCommandState.targetGear = command.gear;

    //Apply the brake, throttle and steer to the command state of the vehicle.
    gVehicle.mCommandState.brakes[0] = CurrentVehicleCommand.brake;
    gVehicle.mCommandState.nbBrakes = 1;
    gVehicle.mCommandState.throttle = CurrentVehicleCommand.throttle;
    gVehicle.mCommandState.steer = CurrentVehicleCommand.steer;
    gVehicle.mTransmissionCommandState.targetGear = CurrentVehicleCommand.gear;

    //Forward integrate the vehicle by a single timestep.
    //Apply substepping at low forward speed to improve simulation fidelity.
    const physx::PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
    const physx::PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
    const physx::PxReal forwardSpeed = linVel.dot(forwardDir);
    const physx::PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
    gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
    gVehicle.step(deltaTime, gVehicleSimulationContext);
}

void Physics::CleanUp()
{
    physx::vehicle2::PxCloseVehicleExtension();

    // clean up vehicle
    gVehicle.destroy();
    // clean up ground plane
    // groundPlane->release();

    // clean up physX
    PX_RELEASE(gMaterial);
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PX_RELEASE(gPhysics);
    if (gPvd)
    {
        physx::PxPvdTransport* transport = gPvd->getTransport();
        gPvd->release();	gPvd = NULL;
        PX_RELEASE(transport);
    }
    PX_RELEASE(gFoundation);

    printf("PhysX destroyed.\n");
}
