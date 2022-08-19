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
#include "ECS/Components/ModelComponent.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"

namespace Lina::ECS
{
    void ModelComponent::SetModel(Graphics::Model* model, Entity owner, Registry& reg)
    {
        m_modelHandle.value = model;
        m_modelHandle.sid   = model->GetSID();
        m_materials.resize(model->m_numMaterials);

        reg.DestroyAllChildren(owner);
        AddNodeRenderer(model, owner, reg, model->GetRootNode());
    }

    void ModelComponent::AddNodeRenderer(Graphics::Model* parentModel, Entity parent, Registry& reg, Graphics::ModelNode* node)
    {
        Entity entity = reg.CreateEntity(node->GetName());
        reg.AddChildToEntity(parent, entity);
        EntityDataComponent& data = reg.get<EntityDataComponent>(entity);
        data.SetLocalTransformation(node->m_localTransform);

        ModelComponent& parentModComp = reg.get<ModelComponent>(parent);
        parentModComp.m_nodeEntities.push_back(entity);

        ModelNodeComponent& modelNodeComp = reg.emplace<ModelNodeComponent>(entity);
        modelNodeComp.m_modelHandle.value = parentModel;
        modelNodeComp.m_modelHandle.sid   = parentModel->GetSID();
        modelNodeComp.m_nodeIndex         = node->m_index;

        for (auto c : node->m_children)
            AddNodeRenderer(parentModel, entity, reg, c);
    }
} // namespace Lina::ECS
