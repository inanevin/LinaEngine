/*
Class: AssimpUtility



Timestamp: 12/24/2021 9:09:36 PM
*/

#pragma once

#ifndef AssimpUtility_HPP
#define AssimpUtility_HPP

// Headers here.
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

namespace Lina::Graphics
{
    extern Matrix  AssimpToLinaMatrix(const aiMatrix4x4& aiMat);
    extern Vector3 AssimpToLinaVector3(const aiVector3D& vec);
} // namespace Lina::Graphics

#endif
