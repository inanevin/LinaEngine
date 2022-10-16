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

#include "Resource/Material.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/ResourceManager.hpp"
#include "PipelineObjects/CommandBuffer.hpp"
#include "Resource/MaterialProperty.hpp"

namespace Lina::Graphics
{
    Material::~Material()
    {
        for (auto p : m_properties)
            delete p;

        m_properties.clear();
        m_dataBuffer.Destroy();
    }

    Resources::Resource* Material::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        Load(archive);
        SetupProperties();
        CheckDescriptorAndBuffer();
        return this;
    }

    Resources::Resource* Material::LoadFromFile(const String& path)
    {
        Serialization::LoadFromFile<Material>(path, *this);
        SetupProperties();
        CheckDescriptorAndBuffer();
        return this;
    }

    void Material::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        LoadFromFile(m_path);
        Save(archive);
    }

    void Material::LoadReferences()
    {
        if (m_shader.value != nullptr)
        {
            SetupProperties();
            CheckDescriptorAndBuffer();
            return;
        }

        FindShader();
    }

    void Material::FindShader()
    {
        auto* manager = Resources::ResourceManager::Get();

        if (manager->Exists<Shader>(m_shader.sid))
            SetShader(manager->GetResource<Shader>(m_shader.sid));
    }

    void Material::SetShader(Shader* shader)
    {
        bool different = false;

        if (m_shader.value != shader)
            different = true;

        m_shader.sid   = shader->GetSID();
        m_shader.value = shader;

        if (different)
        {
            SetupProperties();
            CheckDescriptorAndBuffer();
        }
    }

    void Material::CheckDescriptorAndBuffer()
    {

        if (m_totalPropertySize == 0)
            return;

        m_dataBuffer = Buffer{
            .size        = m_totalPropertySize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
            .memoryUsage = MemoryUsageFlags::CpuToGpu,
        };
        m_dataBuffer.Create();
        m_dataBuffer.boundSet     = &m_descriptor;
        m_dataBuffer.boundBinding = 0;
        m_dataBuffer.boundType    = DescriptorType::UniformBuffer;

        if (m_shader.value == nullptr)
            return;

        m_descriptor = DescriptorSet{
            .setCount = 1,
            .pool     = RenderEngine::Get()->GetDescriptorPool()._ptr,
        };

        m_descriptor.AddLayout(&m_shader.value->GetMaterialSetLayout());
        m_descriptor.Create();
        m_descriptor.BeginUpdate();
        m_descriptor.AddBufferUpdate(m_dataBuffer, m_dataBuffer.size, 0, DescriptorType::UniformBuffer);
        m_descriptor.SendUpdate();
    }

    void Material::SetupProperties()
    {
        if (m_shader.value == nullptr)
            return;

        for (auto p : m_properties)
            delete p;

        m_properties.clear();

        const auto& map = m_shader.value->GetMaterialProperties();

        for (auto pair : map)
        {
            for (auto& prop : pair.second)
            {
                const MaterialPropertyType type = static_cast<MaterialPropertyType>(pair.first);
                switch (type)
                {
                case MaterialPropertyType::Float: {
                    MaterialProperty<float>* p = new MaterialProperty<float>();
                    p->m_name                  = prop;
                    m_properties.push_back(p);
                    break;
                }
                case MaterialPropertyType::Int: {
                    MaterialProperty<int>* p = new MaterialProperty<int>();
                    p->m_name                = prop;
                    m_properties.push_back(p);
                    break;
                }
                case MaterialPropertyType::Bool: {
                    MaterialProperty<bool>* p = new MaterialProperty<bool>();
                    p->m_name                 = prop;
                    m_properties.push_back(p);
                    break;
                }
                case MaterialPropertyType::Vector2: {
                    MaterialProperty<Vector2>* p = new MaterialProperty<Vector2>();
                    p->m_name                    = prop;
                    m_properties.push_back(p);
                    break;
                }
                case MaterialPropertyType::Vector4: {
                    MaterialProperty<Vector3>* p = new MaterialProperty<Vector3>();
                    p->m_name                    = prop;
                    m_properties.push_back(p);
                    break;
                }
                default:
                    LINA_ASSERT(false, "Type not found!");
                }
            }
        }

        m_totalPropertySize = 0;
        for (auto p : m_properties)
            m_totalPropertySize += p->GetTypeSize();
    }

    void Material::BindPipelineAndDescriptors(CommandBuffer& cmd, RenderPassType rpType)
    {
        auto& renderer = RenderEngine::Get()->GetRenderer();

        auto& pipeline = m_shader.value->GetPipeline(rpType);
        pipeline.Bind(cmd, PipelineBindPoint::Graphics);

        if (m_totalPropertySize != 0)
        {
            cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &m_descriptor, 0, nullptr);
            
            uint8* data = new uint8[m_totalPropertySize];
            size_t index = 0;

            // Copy each prop
            for (auto& p : m_properties)
            {
                const size_t typeSize = p->GetTypeSize();
                void* src = p->GetData();
                MEMCPY(data + index, src, typeSize);
                index += typeSize;
            }

            // Update buffer.
            m_dataBuffer.CopyInto(data, m_totalPropertySize);
        }
       
    }

    void Material::SaveToFile()
    {
        if (ApplicationInfo::GetAppMode() != ApplicationMode::Editor)
            return;

        if (Utility::FileExists(m_path))
            Utility::DeleteFileInPath(m_path);

        Serialization::SaveToFile<Material>(m_path, *this);
    }
} // namespace Lina::Graphics
