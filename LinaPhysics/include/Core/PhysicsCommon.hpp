/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Class: PhysicsCommon



Timestamp: 12/20/2021 1:05:42 PM
*/

#pragma once

#ifndef PhysicsCommon_HPP
#define PhysicsCommon_HPP

// Headers here.
#include "Math/Quaternion.hpp"
#include "Math/Vector.hpp"

#ifdef LINA_PHYSICS_BULLET
#include "btBulletDynamicsCommon.h"
#elif LINA_PHYSICS_PHYSX
#include "PxPhysicsAPI.h"

#endif

namespace Lina::Physics
{

#ifdef LINA_PHYSICS_BULLET

    extern btVector3    ToBtVector(const Vector3& v);
    extern btQuaternion ToBtQuat(const Quaternion& q);
    extern Vector3      ToLinaVector(const btVector3& v);
    extern Quaternion   ToLinaQuat(const btQuaternion& q);

#endif
#ifdef LINA_PHYSICS_PHYSX
    extern physx::PxVec2 ToPxVector2(const Vector2& v);
    extern physx::PxVec3 ToPxVector3(const Vector3& v);
    extern physx::PxVec4 ToPxVector4(const Vector4& v);
    extern physx::PxQuat ToPxQuat(const Quaternion& q);
    extern Vector2       ToLinaVector2(const physx::PxVec2& v);
    extern Vector3       ToLinaVector3(const physx::PxVec3& v);
    extern Vector4       ToLinaVector4(const physx::PxVec4& v);
    extern Quaternion    ToLinaQuat(const physx::PxQuat& q);
#endif

} // namespace Lina::Physics

#endif
