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

#include "Resource/Model.hpp"
#include "Resource/ModelNode.hpp"
#include "Resource/Mesh.hpp"
#include "Components/ModelNodeComponent.hpp"
#include "Core/ResourceDataManager.hpp"
#include "Utility/ModelLoader.hpp"
#include "Core/World.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "Memory/Memory.hpp"

namespace Lina::Graphics
{
    Model::~Model()
    {
        if (m_rootNode != nullptr)
            delete m_rootNode;
    }

    void* Model::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        IResource::SetSID(path);
        LoadAssetData();
        ModelLoader::LoadModel(data, dataSize, this);
        return static_cast<void*>(this);
    }

    void* Model::LoadFromFile(const String& path)
    {
        IResource::SetSID(path);
        LoadAssetData();
        ModelLoader::LoadModel(path, this);
        return static_cast<void*>(this);
    }

    void Model::LoadAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        if (!dm->Exists(m_sid))
            SaveAssetData();

        m_assetData.calculateTangent = dm->GetValue<bool>(m_sid, "CalcTang");
        m_assetData.smoothNormals    = dm->GetValue<bool>(m_sid, "SmoothNorm");
        m_assetData.flipUVs          = dm->GetValue<bool>(m_sid, "flipUV");
        m_assetData.flipWinding      = dm->GetValue<bool>(m_sid, "flipWinding");
        m_assetData.triangulate      = dm->GetValue<bool>(m_sid, "tri");
        m_assetData.globalScale      = dm->GetValue<float>(m_sid, "scale");
    }

    void Model::SaveAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        dm->CleanSlate(m_sid);
        dm->SetValue<bool>(m_sid, "CalcTang", m_assetData.calculateTangent);
        dm->SetValue<bool>(m_sid, "SmoothNorm", m_assetData.smoothNormals);
        dm->SetValue<bool>(m_sid, "flipUV", m_assetData.flipUVs);
        dm->SetValue<bool>(m_sid, "flipWinding", m_assetData.flipWinding);
        dm->SetValue<bool>(m_sid, "tri", m_assetData.triangulate);
        dm->SetValue<float>(m_sid, "scale", m_assetData.globalScale);
        dm->Save();
    }

    World::Entity* Model::AddToWorld(World::EntityWorld* w)
    {
        return CreateEntityForNode(nullptr, w, m_rootNode);
    }

    World::Entity* Model::CreateEntityForNode(World::Entity* parent, World::EntityWorld* world, ModelNode* node)
    {
        World::Entity* e = world->CreateEntity(node->GetName());
        if (parent != nullptr)
        {
            parent->AddChild(e);
            e->SetLocalTransformation(node->GetLocalTransform());
        }

        ModelNodeComponent* comp = world->AddComponent<ModelNodeComponent>(e);

        comp->m_modelHandle.sid   = GetSID();
        comp->m_modelHandle.value = this;
        comp->m_nodeIndex         = node->GetNodeIndex();

        for (auto mesh : node->GetMeshes())
        {
            const uint32 slot = static_cast<uint32>(mesh->GetMaterialSlot());

            // Assign default material for now.
            Resources::ResourceHandle<Material> handle;
            handle.value            = RenderEngine::Get()->GetPlaceholderMaterial();
            handle.sid              = handle.value->GetSID();
            comp->m_materials[slot] = handle;
        }

        for (auto c : node->GetChildren())
            CreateEntityForNode(e, world, c);

        return e;
    }
} // namespace Lina::Graphics
