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

#ifndef ModelNode_HPP
#define ModelNode_HPP

#include "Math/AABB.hpp"
#include "Math/Matrix.hpp"
#include "Data/Vector.hpp"
#include "Data/String.hpp"
#include "Resource/StaticMesh.hpp"
#include "Resource/SkinnedMesh.hpp"

namespace Lina::Graphics
{
    class Mesh;

    class ModelNode
    {
    public:
        ModelNode() = default;
        ~ModelNode();

        inline void ClearData()
        {
            m_name              = "";
            m_localTransform    = Matrix();
            m_totalVertexCenter = Vector3::Zero;
            m_aabb              = AABB();
            m_children.clear();
            m_meshes.clear();
        }

        inline const Vector<Mesh*> GetMeshes()
        {
            return m_meshes;
        }

        inline AABB& GetAABB()
        {
            return m_aabb;
        }

        inline Vector3& GetVertexCenter()
        {
            return m_totalVertexCenter;
        }

        inline String& GetName()
        {
            return m_name;
        }

        inline uint32 GetNodeIndex()
        {
            return m_index;
        }

        inline Matrix& GetLocalTransform()
        {
            return m_localTransform;
        }

        inline const Vector<ModelNode*>& GetChildren()
        {
            return m_children;
        }

        template <typename Archive>
        void Save(Archive& archive)
        {
            archive(m_aabb, m_localTransform, m_totalVertexCenter, m_name, m_index);

            Vector<uint32> childrenIndices;

            for (auto& c : m_children)
                childrenIndices.push_back(c->m_index);

            archive(childrenIndices);

            const uint32 meshesSize = static_cast<uint32>(m_meshes.size());
            archive(meshesSize);

            for (auto* m : m_meshes)
            {
                const uint8 mt = static_cast<uint8>(m->GetMeshType());
                archive(mt);
                archive(*m);
            }
        }

        template <typename Archive>
        void Load(Archive& archive)
        {
            archive(m_aabb, m_localTransform, m_totalVertexCenter, m_name, m_index);
            archive(m_childrenIndices);

            uint32 meshesSize = 0;
            archive(meshesSize);

            for (uint32 i = 0; i < meshesSize; i++)
            {
                uint8 mtint = 0;
                archive(mtint);
                MeshType mt = static_cast<MeshType>(mtint);
                Mesh*    m  = nullptr;

                if (mt == MeshType::StaticMesh)
                    m = new StaticMesh();
                else
                    m = new SkinnedMesh();

                archive(*m);
                m_meshes.push_back(m);
            }
        }

    private:
        friend class ModelLoader;
        friend class Model;

        AABB               m_aabb;
        Matrix             m_localTransform;
        Vector3            m_totalVertexCenter = Vector3::Zero;
        String             m_name              = "";
        Vector<ModelNode*> m_children;
        Vector<uint32>     m_childrenIndices;
        Vector<Mesh*>      m_meshes;
        uint32             m_index = 0;
    };
} // namespace Lina::Graphics

#endif
