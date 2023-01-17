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

#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Graphics/Utility/ModelLoader.hpp"
#include "World/Core/World.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Memory/Memory.hpp"

namespace Lina::Graphics
{
    Model::~Model()
    {
        if (m_rootNode != nullptr)
            delete m_rootNode;
    }

    Resources::Resource* Model::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        LoadFromArchive(archive);

        for (auto* n : m_nodes)
        {
            auto& meshes = n->GetMeshes();

            for (auto* m : meshes)
            {
                m->GenerateBuffers();
                m->ClearInitialBuffers();
            }
        }

        return this;
    }

    Resources::Resource* Model::LoadFromFile(const char* path)
    {
        LoadAssetData();

        if (m_rootNode == nullptr)
        {
            ModelLoader::LoadModel(m_subsystems.renderEngine, path, this);
            SaveAssetData();
        }

        for (auto* n : m_nodes)
        {
            auto& meshes = n->GetMeshes();

            for (auto* m : meshes)
            {
                m->GenerateBuffers();
                m->ClearInitialBuffers();
            }
        }

        return this;
    }

    void Model::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        LoadAssetData();

        if (m_rootNode == nullptr)
            ModelLoader::LoadModel(m_subsystems.renderEngine, m_path, this);

        SaveToArchive(archive);
    }

    void Model::SaveToArchive(Serialization::Archive<OStream>& archive)
    {
        archive(m_assetData.calculateTangent);
        archive(m_assetData.smoothNormals);
        archive(m_assetData.flipUVs);
        archive(m_assetData.flipWinding);
        archive(m_assetData.triangulate);
        archive(m_assetData.globalScale);
        archive(m_numMaterials, m_numAnims, m_numMeshes, m_numBones, m_numVertices);

        const uint32 nodeSize = static_cast<uint32>(m_nodes.size());
        archive(nodeSize);

        for (uint32 i = 0; i < nodeSize; i++)
            archive(*m_nodes[i]);
    }

    void Model::LoadFromArchive(Serialization::Archive<IStream>& archive)
    {
        archive(m_assetData.calculateTangent);
        archive(m_assetData.smoothNormals);
        archive(m_assetData.flipUVs);
        archive(m_assetData.flipWinding);
        archive(m_assetData.triangulate);
        archive(m_assetData.globalScale);
        archive(m_numMaterials, m_numAnims, m_numMeshes, m_numBones, m_numVertices);

        uint32 nodeSize = 0;
        archive(nodeSize);

        for (uint32 i = 0; i < nodeSize; i++)
        {
            ModelNode* node = new ModelNode(m_subsystems.renderEngine);
            archive(*node);
            m_nodes.push_back(node);
        }

        if (nodeSize == 0)
            return;

        m_rootNode = m_nodes[0];

        // Assign childrens
        for (auto* node : m_nodes)
        {
            for (auto& childrenIndex : node->m_childrenIndices)
                node->m_children.push_back(m_nodes[childrenIndex]);
        }
    }

    World::Entity* Model::AddToWorld(World::EntityWorld* w)
    {
        LINA_ASSERT(w != nullptr, "World doesn't exist!");
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

        if (!node->GetMeshes().empty())
        {
            ModelNodeComponent* comp = world->AddComponent<ModelNodeComponent>(e);

            comp->m_modelHandle.sid   = GetSID();
            comp->m_modelHandle.value = this;
            comp->m_nodeIndex         = node->GetNodeIndex();
            comp->m_stubModel         = m_subsystems.renderEngine->GetPlaceholderModel();
            comp->m_stubModelNode     = m_subsystems.renderEngine->GetPlaceholderModelNode();
            comp->m_stubMesh          = m_subsystems.renderEngine->GetPlaceholderMesh();
            comp->m_stubMaterial      = m_subsystems.renderEngine->GetPlaceholderMaterial();

            for (auto mesh : node->GetMeshes())
            {
                const uint32 slot = static_cast<uint32>(mesh->GetMaterialSlot());

                // Assign default material for now.
                Resources::ResourceHandle<Material> handle;
                handle.value            = m_subsystems.renderEngine->GetPlaceholderMaterial();
                handle.sid              = handle.value->GetSID();
                comp->m_materials[slot] = handle;
            }
        }

        for (auto c : node->GetChildren())
            CreateEntityForNode(e, world, c);

        return e;
    }

} // namespace Lina::Graphics
