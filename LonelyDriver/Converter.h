#pragma once

#ifndef __INCLUDE_PHYSX__
#define __INCLUDE_PHYSX__
#include "PxPhysicsAPI.h"
#endif

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#define __INCLUDE_GLM_MATRIX_TRANSFORM_HPP__
#include <glm/gtc/matrix_transform.hpp>
#endif

// code sourced from: https://github.com/Caspila/GUInity/blob/master/Source/Converter.cpp

/** Convert glm::mat4 to PhysX::PxMat44
 @param[in] mat4 The glm::mat4
 @return Converted PhysX::PxMat44 */
physx::PxMat44 glmMat4ToPhysxMat4(const glm::mat4 & mat4)
{
    physx::PxMat44 newMat;

    newMat[0][0] = mat4[0][0];
    newMat[0][1] = mat4[0][1];
    newMat[0][2] = mat4[0][2];
    newMat[0][3] = mat4[0][3];

    newMat[1][0] = mat4[1][0];
    newMat[1][1] = mat4[1][1];
    newMat[1][2] = mat4[1][2];
    newMat[1][3] = mat4[1][3];

    newMat[2][0] = mat4[2][0];
    newMat[2][1] = mat4[2][1];
    newMat[2][2] = mat4[2][2];
    newMat[2][3] = mat4[2][3];

    newMat[3][0] = mat4[3][0];
    newMat[3][1] = mat4[3][1];
    newMat[3][2] = mat4[3][2];
    newMat[3][3] = mat4[3][3];


    return newMat;
}

/** Convert PhysX::PxMat44 to glm::mat4
 @param[in] mat4 The PhysX::PxMat44
 @return Converted glm::mat4
 */
glm::mat4 PhysXMat4ToglmMat4(const physx::PxMat44& mat4)
{
    glm::mat4 newMat;

    newMat[0][0] = mat4[0][0];
    newMat[0][1] = mat4[0][1];
    newMat[0][2] = mat4[0][2];
    newMat[0][3] = mat4[0][3];

    newMat[1][0] = mat4[1][0];
    newMat[1][1] = mat4[1][1];
    newMat[1][2] = mat4[1][2];
    newMat[1][3] = mat4[1][3];

    newMat[2][0] = mat4[2][0];
    newMat[2][1] = mat4[2][1];
    newMat[2][2] = mat4[2][2];
    newMat[2][3] = mat4[2][3];

    newMat[3][0] = mat4[3][0];
    newMat[3][1] = mat4[3][1];
    newMat[3][2] = mat4[3][2];
    newMat[3][3] = mat4[3][3];

    return newMat;
}

/** Convert PhysX::PxVec3 to glm::vec3
 @param[in] The PhysX::PxVec3
 @return Converted glm::vec3
 */
glm::vec3 PhysXVec3ToglmVec3(const physx::PxVec3& vec3)
{
    glm::vec3 newVec;

    newVec.x = vec3.x;
    newVec.y = vec3.y;
    newVec.z = vec3.z;

    return newVec;
}

/** Convert glm::vec3 to PhysX::PxVec3
 @param[in] The glm::vec3
 @return    Converted PhysX::PxVec3
 */
physx::PxVec3 glmVec3ToPhysXVec3(const glm::vec3& vec3)
{
    physx::PxVec3 newVec;

    newVec.x = vec3.x;
    newVec.y = vec3.y;
    newVec.z = vec3.z;

    return newVec;
}
