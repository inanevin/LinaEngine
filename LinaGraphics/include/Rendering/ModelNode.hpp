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
Class: ModelNode



Timestamp: 12/24/2021 9:00:02 PM
*/

#pragma once

#ifndef ModelNode_HPP
#define ModelNode_HPP

// Headers here.
#include "Math/Matrix.hpp"
#include "Utility/StringId.hpp"
#include "Resources/ResourceHandle.hpp"
#include "Rendering/Material.hpp"
#include "Math/AABB.hpp"
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <memory>
#include <vector>

struct aiNode;
struct aiScene;

namespace Lina
{
    namespace ECS
    {
        class ModelNodeSystem;
        class FrustumSystem;
    }
} // namespace Lina

namespace Lina::Graphics
{
    class Mesh;
    class Model;
    class ModelLoader;

    class ModelNode
    {

    public:
        ModelNode(){};
        ~ModelNode();

        void                             FillNodeHierarchy(const aiNode* node, const aiScene* scene, Model* parentModel);
        inline const std::vector<Mesh*>& GetMeshes() const
        {
            return m_meshes;
        }
        inline const std::string& GetName() const
        {
            return m_name;
        }
        inline const std::vector<ModelNode*>& GetChildren() const
        {
            return m_children;
        }
        inline int GetIndexInParentHierarchy()
        {
            return m_nodeIndexInParentHierarchy;
        }
        inline Vector3 GetTotalVertexCenter()
        {
            return m_totalVertexCenter;
        }
        inline AABB& GetAABB()
        {
            return m_aabb;
        }

    private:
        void Clear()
        {
            m_name           = "";
            m_localTransform = Matrix();
            m_children.clear();
        }

    private:
        friend class ECS::ModelNodeSystem;
        friend class ECS::FrustumSystem;
        friend class Graphics::ModelLoader;
        friend class cereal::access;

        int                     m_nodeIndexInParentHierarchy = 0;
        std::vector<Mesh*>      m_meshes;
        std::string             m_name              = "";
        Vector3                 m_totalVertexCenter = Vector3::Zero;
        AABB                    m_aabb;
        Matrix                  m_localTransform;
        std::vector<ModelNode*> m_children;
    };
} // namespace Lina::Graphics

#endif
