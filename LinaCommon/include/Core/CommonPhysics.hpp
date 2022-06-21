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

    extern String COLLISION_SHAPES[4];

    enum class CollisionShape : uint8
    {
        Box        = 0,
        Sphere     = 1,
        Capsule    = 2,
        ConvexMesh = 3,
    };
} // namespace Lina::Physics

#endif
