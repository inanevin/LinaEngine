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

#pragma once

#ifndef Mesh_HPP
#define Mesh_HPP

#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Data/Vertex.hpp"
#include "Math/AABB.hpp"
#include "PipelineObjects/Buffer.hpp"

namespace Lina
{
    namespace ECS
    {
        struct ModelComponent;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class Mesh
    {
    public:
        Mesh() = default;
        virtual ~Mesh();

        inline const Vector<Vertex>& GetVertices()
        {
            return m_vertices;
        }

        inline const Buffer& GetGPUVtxBuffer()
        {
            return m_gpuVtxBuffer;
        }

        inline int GetMaterialSlot()
        {
            return m_materialSlot;
        }

    protected:
    private:
        void AddVertex(const Vector3& pos, const Vector3& normal);
        void AddIndices(uint32 i1, uint32 i2, uint32 i3);
        void GenerateBuffers();

    private:
    protected:
        friend class ModelLoader;
        friend struct ECS::ModelComponent;

        Buffer            m_gpuVtxBuffer;
        Vector<Vector3ui> m_indices;
        Vector<Vertex>    m_vertices;
        Vector3           m_vertexCenter = Vector3::Zero;
        AABB              m_aabb;
        int               m_materialSlot = 0;
        String            m_name         = "";
    };

} // namespace Lina::Graphics

#endif
