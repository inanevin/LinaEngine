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
#include "Core/ResourceDataManager.hpp"
#include "Utility/ModelLoader.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "Memory/Memory.hpp"
#include "Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

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

        // m_vertices.resize(3);
        // m_vertices[0].pos = {1.f, 1.f, 0.0f};
        // m_vertices[1].pos = {-1.f, 1.f, 0.0f};
        // m_vertices[2].pos = {0.f, -1.f, 0.0f};
        // 
        // // vertex colors, all green
        // m_vertices[0].color = {0.f, 1.f, 0.0f}; // pure green
        // m_vertices[1].color = {0.f, 1.f, 0.0f}; // pure green
        // m_vertices[2].color = {0.f, 1.f, 0.0f}; // pure green
        // 
        // AllocateBuffers(this);

        return static_cast<void*>(this);
    }

    void Model::LoadAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        if (!dm->Exists(m_sid))
            SaveAssetData();

        m_assetData.calculateTangent = Resources::ResourceDataManager::Get()->GetValue<bool>(m_sid, "CalcTang");
        m_assetData.smoothNormals    = Resources::ResourceDataManager::Get()->GetValue<bool>(m_sid, "SmoothNorm");
        m_assetData.flipUVs          = Resources::ResourceDataManager::Get()->GetValue<bool>(m_sid, "flipUV");
        m_assetData.flipWinding      = Resources::ResourceDataManager::Get()->GetValue<bool>(m_sid, "flipWinding");
        m_assetData.triangulate      = Resources::ResourceDataManager::Get()->GetValue<bool>(m_sid, "tri");
        m_assetData.globalScale      = Resources::ResourceDataManager::Get()->GetValue<float>(m_sid, "scale");
    }

    void Model::SaveAssetData()
    {
        Resources::ResourceDataManager::Get()->SetValue<bool>(m_sid, "CalcTang", m_assetData.calculateTangent);
        Resources::ResourceDataManager::Get()->SetValue<bool>(m_sid, "SmoothNorm", m_assetData.smoothNormals);
        Resources::ResourceDataManager::Get()->SetValue<bool>(m_sid, "flipUV", m_assetData.flipUVs);
        Resources::ResourceDataManager::Get()->SetValue<bool>(m_sid, "flipWinding", m_assetData.flipWinding);
        Resources::ResourceDataManager::Get()->SetValue<bool>(m_sid, "tri", m_assetData.triangulate);
        Resources::ResourceDataManager::Get()->SetValue<float>(m_sid, "scale", m_assetData.globalScale);
        Resources::ResourceDataManager::Get()->Save();
    }
} // namespace Lina::Graphics
