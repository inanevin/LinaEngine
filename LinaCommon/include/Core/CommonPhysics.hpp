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
Class: CommonPhysics



Timestamp: 12/25/2021 11:44:20 AM
*/

#pragma once

#ifndef CommonPhysics_HPP
#define CommonPhysics_HPP

// Headers here.
#include "SizeDefinitions.hpp"

#include <Data/String.hpp>

namespace Lina::Physics
{
    extern String SIMULATION_TYPES[3];

    enum class SimulationType : uint8
    {
        None    = 0,
        Static  = 1,
        Dynamic = 2,
    };

#ifdef LINA_PHYSICS_BULLET
    extern String COLLISION_SHAPES[4];

    enum class CollisionShape : uint8
    {
        Box        = 0,
        Sphere     = 1,
        Cylinder   = 2,
        Capsule    = 3,
        ConvexMesh = 4,
    };
#elif LINA_PHYSICS_PHYSX
    extern String COLLISION_SHAPES[4];

    enum class CollisionShape : uint8
    {
        Box        = 0,
        Sphere     = 1,
        Capsule    = 2,
        ConvexMesh = 3,
    };
#endif
} // namespace Lina::Physics

#endif
