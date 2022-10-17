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

#define DEBUG_RELOAD_PROPERTIES

namespace Lina::Graphics
{
    Material::~Material()
    {
        for (auto p : m_properties)
            delete p;

        m_descriptorPool.Destroy();
        m_properties.clear();
        m_dataBuffer.Destroy();
    }

    Resources::Resource* Material::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        Load(archive);
        CreateBuffer();
        return this;
    }

    Resources::Resource* Material::LoadFromFile(const String& path)
    {
        Serialization::LoadFromFile<Material>(path, *this);
        CreateBuffer();
        return this;
    }

    void Material::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        Serialization::LoadFromFile<Material>(m_path, *this);
        Save(archive);
    }

    void Material::LoadReferences()
    {
        if (m_shader.value == nullptr)
        {
            auto* manager = Resources::ResourceManager::Get();

            if (manager->Exists<Shader>(m_shader.sid))
                SetShader(manager->GetResource<Shader>(m_shader.sid));
            else
                SetShader(RenderEngine::Get()->GetEngineShader(EngineShaderType::LitStandard));
        }
        else
        {
            if (m_properties.empty())
            {
                SetupProperties();
                ChangedShader();
            }
        }

#ifdef DEBUG_RELOAD_PROPERTIES
        SetupProperties();
        ChangedShader();
#endif
    }

    void Material::SetShader(Shader* shader)
    {
        if (shader == m_shader.value)
            return;

        m_shader.sid   = shader->GetSID();
        m_shader.value = shader;

        SetupProperties();
        ChangedShader();
    }

    void Material::CreateBuffer()
    {
        if (m_dataBuffer._ptr == nullptr)
        {
            m_dataBuffer = Buffer{
                .size        = m_totalPropertySize == 0 ? sizeof(float) * 4 : m_totalPropertySize,
                .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
                .memoryUsage = MemoryUsageFlags::CpuToGpu,
            };

            m_dataBuffer.Create();
            m_dataBuffer.boundSet     = &m_descriptor;
            m_dataBuffer.boundBinding = 0;
            m_dataBuffer.boundType    = DescriptorType::UniformBuffer;
        }
    }

    void Material::ChangedShader()
    {
        if (m_descriptorPool._ptr != nullptr)
            m_descriptorPool.Destroy();

        m_descriptorPool = DescriptorPool{
            .maxSets = 10,
            .flags   = DescriptorPoolCreateFlags::None,
        };
        m_descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 10).AddPoolSize(DescriptorType::UniformBufferDynamic, 2).AddPoolSize(DescriptorType::CombinedImageSampler, 2).Create(false);

        if (!m_shader.value->GetMaterialSetLayout().bindings.empty())
        {
            m_descriptor = DescriptorSet{
                .setCount = 1,
                .pool     = m_descriptorPool._ptr,
            };
            m_descriptor.AddLayout(&m_shader.value->GetMaterialSetLayout());
            m_descriptor.Create();

            m_descriptor.BeginUpdate();

            if (m_shader.value->GetMaterialSetLayout().bindings[0].type == DescriptorType::UniformBuffer)
                m_descriptor.AddBufferUpdate(m_dataBuffer, m_dataBuffer.size, 0, DescriptorType::UniformBuffer);

            for (auto& pair : m_textures)
                m_descriptor.AddTextureUpdate(pair.first, pair.second.value);

            m_descriptor.SendUpdate();
        }
    }

    void Material::SetupProperties()
    {
        for (auto p : m_properties)
            delete p;

        m_properties.clear();

        const auto& map = m_shader.value->GetMaterialProperties();

        for (auto pair : map)
        {
            for (auto& prop : pair.second)
            {
                const MaterialPropertyType type = static_cast<MaterialPropertyType>(pair.first);
                AddProperty(type, prop);
            }
        }

        m_totalPropertySize = 0;
        for (auto p : m_properties)
            m_totalPropertySize += static_cast<uint8>(p->GetTypeSize());

        m_textures.clear();

        Texture* defaultLina = RenderEngine::Get()->GetEngineTexture(EngineTextureType::DefaultLina);

        for (auto& b : m_shader.value->GetMaterialSetLayout().bindings)
        {
            if (b.type == DescriptorType::CombinedImageSampler)
            {
                m_textures[b.binding].sid   = defaultLina->GetSID();
                m_textures[b.binding].tid   = GetTypeID<Texture>();
                m_textures[b.binding].value = defaultLina;
            }
        }

        if (m_descriptor._ptr != nullptr)
        {
            m_descriptor.BeginUpdate();

            for (auto& pair : m_textures)
                m_descriptor.AddTextureUpdate(pair.first, pair.second.value);
            m_descriptor.SendUpdate();
        }
    }

    void Material::BindPipelineAndDescriptors(CommandBuffer& cmd, RenderPassType rpType)
    {
        auto& renderer = RenderEngine::Get()->GetRenderer();
        auto& pipeline = m_shader.value->GetPipeline(rpType);
        pipeline.Bind(cmd, PipelineBindPoint::Graphics);

        if (m_totalPropertySize != 0 || !m_textures.empty())
        {
            cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &m_descriptor, 0, nullptr);

            if (m_totalPropertySize != 0 && m_propertiesDirty)
            {
                uint8* data  = new uint8[m_totalPropertySize];
                size_t index = 0;

                // Copy each prop
                for (auto& p : m_properties)
                {
                    const size_t typeSize = p->GetTypeSize();
                    void*        src      = p->GetData();
                    MEMCPY(data + index, src, typeSize);
                    index += typeSize;
                }

                // Update buffer.
                m_dataBuffer.CopyInto(data, m_totalPropertySize);
                m_propertiesDirty = false;
            }

            if (!m_runtimeDirtyTextures.empty())
            {
                m_descriptor.BeginUpdate();

                for (auto& pair : m_runtimeDirtyTextures)
                {
                    m_descriptor.AddTextureUpdate(pair.first, pair.second);
                    m_textures[pair.first].sid   = pair.second->GetSID();
                    m_textures[pair.first].value = pair.second;
                }

                m_descriptor.SendUpdate();

                m_runtimeDirtyTextures.clear();
            }
        }
    }

    void Material::SetTexture(uint32 binding, Texture* txt)
    {
        if (!m_textures.contains(binding))
        {
            LINA_ERR("[Material] -> Can't set the texture because the binding doesn't exists: {0} {1}", binding, m_path.c_str());
            return;
        }

        m_runtimeDirtyTextures[binding] = txt;
    }

    void Material::SaveToFile()
    {
        if (ApplicationInfo::GetAppMode() != ApplicationMode::Editor)
            return;

        if (Utility::FileExists(m_path))
            Utility::DeleteFileInPath(m_path);

        Serialization::SaveToFile<Material>(m_path, *this);
    }

    MaterialPropertyBase* Material::AddProperty(MaterialPropertyType type, const String& name)
    {
        MaterialPropertyBase* p = nullptr;
        switch (type)
        {
        case MaterialPropertyType::Float: {
            p = new MaterialProperty<float>();
            break;
        }
        case MaterialPropertyType::Int: {
            p = new MaterialProperty<int>();
            break;
        }
        case MaterialPropertyType::Bool: {
            p = new MaterialProperty<bool>();
            break;
        }
        case MaterialPropertyType::Vector2: {
            p = new MaterialProperty<Vector2>();
            break;
        }
        case MaterialPropertyType::Vector4: {
            p = new MaterialProperty<Vector3>();
            break;
        }
        default:
            LINA_ASSERT(false, "Type not found!");
        }

        p->m_name = name;
        p->m_type = type;
        m_properties.push_back(p);
        return p;
    }
} // namespace Lina::Graphics
