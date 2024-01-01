#include "Physics.h"
#include "Converter.h"
#include "Light.h"
#include "Drawable.h"

#define PVD_HOST "127.0.0.1"

physx::PxDefaultAllocator		Physics::gAllocator;
physx::PxDefaultErrorCallback	Physics::gErrorCallback;
physx::PxFoundation*            Physics::gFoundation      = NULL;
physx::PxPhysics*               Physics::gPhysics         = NULL;
physx::PxDefaultCpuDispatcher*  Physics::gDispatcher      = NULL;
physx::PxScene*                 Physics::gScene           = NULL;
physx::PxMaterial*              Physics::gMaterial        = NULL;
physx::PxPvd*                   Physics::gPvd             = NULL;


void Physics::Init()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
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

    Physics::CreatePlane();
}

void Physics::CreatePlane()
{
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
    groundPlane->setName("ground");
    gScene->addActor(*groundPlane);


}

void Physics::AddActor(const physx::PxGeometryType::Enum& geoType, Drawable* object)
{
    // material
    // RigidActor type: PxRigidStatic | PxRigidDynamic 
    // Pxshape
    // Transform

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    if (geoType == physx::PxGeometryType::eBOX)
    {
        // create a box
        Cube* cubeObject = static_cast<Cube*>(object);

        physx::PxTransform t = physx::PxTransform(glmVec3ToPhysXVec3(cubeObject->Position));
        
        // physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(cubeObject->hx / 2.f, cubeObject->hy / 2.f, cubeObject->hz / 2.f), *gMaterial);
        physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(cubeObject->m_HalfLength.x, cubeObject->m_HalfLength.y, cubeObject->m_HalfLength.z), *gMaterial);
        physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(t);
        body->attachShape(*shape);
        body->setName(cubeObject->Name.c_str());
        physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
        gScene->addActor(*body);
        shape->release();
    }
}

void Physics::Step(float deltaTime, std::unordered_map<std::string, glm::mat4>& objectGlobalPoses, Light* light)
{
    gScene->simulate(deltaTime);
    gScene->fetchResults(true);

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
                newModelMat = PhysXMat4ToglmMat4(shapePose);
                objectGlobalPoses[actors[i]->getName()] = newModelMat;

                if (actors[i]->getName() == light->Name)
                {
                    light->Position = PhysXVec3ToglmVec3(physx::PxShapeExt::getGlobalPose(*shapes[j], *actors[i]).p);
                }
            }
        }
    }
}

void Physics::CleanUp()
{
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
