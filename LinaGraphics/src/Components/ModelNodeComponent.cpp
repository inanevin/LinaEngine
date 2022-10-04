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

#include "Components/ModelNodeComponent.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/ModelNode.hpp"

namespace Lina::Graphics
{
    AABB& ModelNodeComponent::GetAABB()
    {
        Model*     placeholderModel = RenderEngine::Get()->GetPlaceholderModel();
        ModelNode* placeholderNode  = RenderEngine::Get()->GetPlaceholderModelNode();
        Model*     model            = m_modelHandle.IsValid() ? m_modelHandle.value : placeholderModel;
        ModelNode* node             = m_modelHandle.IsValid() ? model->GetNodes()[m_nodeIndex] : placeholderNode;
        return node->GetAABB();
    }

    Bitmask16 ModelNodeComponent::GetDrawPasses()
    {
        Bitmask16 mask;

        for (auto& m : m_materials)
        {
            if (m.second.IsValid() && m.second.value->GetShaderHandle().IsValid())
                mask.Set(m.second.value->GetShaderHandle().value->GetDrawPassMask().GetValue());
        }

        return mask;
    }

    Vector<MeshMaterialPair> ModelNodeComponent::GetMeshMaterialPairs()
    {
        Vector<MeshMaterialPair> pairs;

        // Only return single placeholder mesh material pair if not valid yet.
        if (!m_modelHandle.IsValid())
        {
            MeshMaterialPair p;
            p.mesh     = RenderEngine::Get()->GetPlaceholderMesh();
            p.material = RenderEngine::Get()->GetPlaceholderMaterial();
            pairs.push_back(p);
            return pairs;
        }

        const auto& meshes = m_modelHandle.value->GetNodes()[m_nodeIndex]->GetMeshes();

        uint32 index = 0;
        for (auto m : meshes)
        {
            MeshMaterialPair p;
            p.mesh     = m;
            p.material = m_materials[index].IsValid() ? m_materials[index].value : RenderEngine::Get()->GetPlaceholderMaterial();
            pairs.push_back(p);
            index++;
        }

        return pairs;
    }
} // namespace Lina::Graphics
