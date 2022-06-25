/*
Class: StaticMesh



Timestamp: 12/24/2021 8:54:24 PM
*/

#pragma once

#ifndef StaticMesh_HPP
#define StaticMesh_HPP

// Headers here.
#include "Rendering/Mesh.hpp"

namespace Lina::Graphics
{
    class StaticMesh : public Mesh
    {

    public:
        StaticMesh() = default;
        virtual ~StaticMesh() = default;

    private:
    };
} // namespace Lina::Graphics

#endif
