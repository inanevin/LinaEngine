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
    enum class MeshType
    {
        StaticMesh,
        SkinnedMesh
    };

    class Mesh
    {
    public:
        Mesh() = default;
        virtual ~Mesh();

        inline uint32 GetVertexSize()
        {
            return m_vtxSize;
        }

        inline uint32 GetIndexSize()
        {
            return m_indexSize;
        }

        inline const Buffer& GetGPUVtxBuffer()
        {
            return m_gpuVtxBuffer;
        }

        inline const Buffer& GetGPUIndexBuffer()
        {
            return m_gpuIndexBuffer;
        }

        inline int GetMaterialSlot()
        {
            return m_materialSlot;
        }

        inline const Vector<Vertex>& GetVertices() const
        {
            return m_vertices;
        }

        inline const Vector<uint32>& GetIndices() const
        {
            return m_indices;
        }

        inline const String& GetName() const
        {
            return m_name;
        }

        virtual MeshType GetMeshType() = 0;

        template <typename Archive> void Save(Archive& archive)
        {
            archive(m_vertexCenter, m_aabb, m_materialSlot, m_name);
            archive(m_indices, m_vertices);
            SaveToArchive(archive);
        }

        template <typename Archive> void Load(Archive& archive)
        {
            archive(m_vertexCenter, m_aabb, m_materialSlot, m_name);
            archive(m_indices, m_vertices);
            LoadFromArchive(archive);
        }

        virtual void SaveToArchive(Serialization::Archive<OStream>& archive){};
        virtual void LoadFromArchive(Serialization::Archive<IStream>& archive){};

    protected:
    private:
        void AddVertex(const Vector3& pos, const Vector3& normal, const Vector2& uv);
        void AddIndices(uint32 i1);
        void GenerateBuffers();
        void ClearInitialBuffers();

    private:
    protected:
        friend class ModelLoader;
        friend class ModelNode;
        friend class Model;
        friend struct ECS::ModelComponent;

        Buffer         m_gpuVtxBuffer;
        Buffer         m_gpuIndexBuffer;
        Buffer         m_cpuVtxBuffer;
        Buffer         m_cpuIndexBuffer;
        Vector<uint32> m_indices;
        Vector<Vertex> m_vertices;
        Vector3        m_vertexCenter = Vector3::Zero;
        AABB           m_aabb;
        int            m_materialSlot = 0;
        String         m_name         = "";
        uint32         m_vtxSize      = 0;
        uint32         m_indexSize    = 0;
    };

} // namespace Lina::Graphics

#endif
