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

// #define DEBUG_RELOAD_PROPERTIES

namespace Lina::Graphics
{
    Material::~Material()
    {
        for (auto p : m_properties)
            delete p;

        for (auto p : m_savedProperties)
            delete p;

        m_descriptorPool.Destroy();
        m_properties.clear();
        m_savedProperties.clear();
        m_textures.clear();
        m_savedTextures.clear();
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

        SetupProperties();
        ChangedShader();
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
                .size        = m_totalPropertySize == 0 ? sizeof(float) * 10 : m_totalPropertySize,
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
            .flags   = DescriptorPoolCreateFlags::UpdateAfterBind,
        };
        m_descriptorPool.AddPoolSize(DescriptorType::UniformBuffer, 2).AddPoolSize(DescriptorType::UniformBufferDynamic, 2).AddPoolSize(DescriptorType::CombinedImageSampler, 2).Create(false);

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
                m_descriptor.AddTextureUpdate(pair.binding, pair.handle.value);

            m_descriptor.SendUpdate();
        }
    }

    void Material::SetupProperties()
    {
        for (auto p : m_properties)
            delete p;

        m_properties.clear();
        m_textures.clear();
        Texture* defaultLina = RenderEngine::Get()->GetEngineTexture(EngineTextureType::DefaultLina);

        const auto& vec = m_shader.value->GetReflectedProperties();

        for (const auto& reflectedProperty : vec)
        {
            if (reflectedProperty.descriptorType == DescriptorType::UniformBuffer)
            {
                auto* p = CreateProperty(reflectedProperty.propertyType, reflectedProperty.name);
                m_properties.push_back(p);

                // If a property with this name & type was saved retrieve value
                for (auto& saved : m_savedProperties)
                {
                    if (p->m_type == saved->m_type && p->m_name.compare(saved->m_name.c_str()) == 0)
                    {
                        p->CopyValueFrom(saved);
                        break;
                    }
                }
            }
            else if (reflectedProperty.descriptorType == DescriptorType::CombinedImageSampler)
            {
                TextureProperty prop;
                prop.binding      = reflectedProperty.descriptorBinding;
                prop.name         = reflectedProperty.name;
                prop.handle.sid   = defaultLina->GetSID();
                prop.handle.tid   = GetTypeID<Texture>();
                prop.handle.value = defaultLina;

                // If a property with this name & type was saved retrieve value
                for (auto& t : m_savedTextures)
                {
                    if (prop.binding == t.binding && prop.name.compare(t.name.c_str()) == 0)
                    {
                        // Only assign if textures available, else leave at default.
                        if (t.handle.value != nullptr)
                        {
                            prop.handle.sid   = t.handle.sid;
                            prop.handle.value = t.handle.value;
                        }
                        else
                        {
                            Texture* text = Resources::ResourceManager::Get()->GetResource<Texture>(t.handle.sid);
                            if (text != nullptr)
                            {
                                prop.handle.sid   = t.handle.sid;
                                prop.handle.value = text;
                            }
                        }

                        break;
                    }
                }

                m_textures.push_back(prop);
            }
        }

        m_totalPropertySize = 0;

        for (auto p : m_properties)
            m_totalPropertySize += p->GetTypeSize();

        m_totalAlignedSize = GetPropertiesTotalAlignedSize();

        if (m_descriptor._ptr != nullptr)
        {
            m_descriptor.BeginUpdate();

            for (auto& pair : m_textures)
                m_descriptor.AddTextureUpdate(pair.binding, pair.handle.value);
            m_descriptor.SendUpdate();
        }
    }

    uint32 Material::GetPropertyTypeAlignment(MaterialPropertyType type)
    {
        if (type == MaterialPropertyType::Vector4 || type == MaterialPropertyType::Mat4)
            return static_cast<uint32>(16);
        else if (type == MaterialPropertyType::Vector2)
            return static_cast<uint32>(8);
        return static_cast<uint32>(4);
    }

    uint32 Material::GetPropertiesTotalAlignedSize()
    {
        uint32 offset = 0;

        for (auto& p : m_properties)
        {
            const uint32 typeSize  = p->GetTypeSize();
            const uint32 alignment = GetPropertyTypeAlignment(p->GetType());
            void*        src       = p->GetData();

            if (offset != 0)
            {
                auto mod = offset % alignment;

                if (mod != 0)
                {
                    if (offset < alignment)
                        offset = alignment;
                    else
                        offset = static_cast<uint32>(Math::CeilToInt(static_cast<float>(offset) / static_cast<float>(alignment)) * alignment);
                }
            }

            offset += typeSize;
        }

        return offset;
    }

    void Material::BindPipelineAndDescriptors(CommandBuffer& cmd, RenderPassType rpType)
    {
        auto& renderer = RenderEngine::Get()->GetRenderer();
        auto& pipeline = m_shader.value->GetPipeline(rpType);
        pipeline.Bind(cmd, PipelineBindPoint::Graphics);
        cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &m_descriptor, 0, nullptr);
    }

    void Material::CheckUpdatePropertyBuffers()
    {
        if (m_totalPropertySize != 0 || !m_textures.empty())
        {
            if (m_propertiesDirty)
            {
                uint8* data = new uint8[m_totalAlignedSize];

                uint32 offset = 0;

                for (auto& p : m_properties)
                {
                    const uint32 typeSize  = p->GetTypeSize();
                    const uint32 alignment = GetPropertyTypeAlignment(p->GetType());
                    void*        src       = p->GetData();

                    if (offset != 0)
                    {
                        auto mod = offset % alignment;

                        if (mod != 0)
                        {
                            if (offset < alignment)
                                offset = alignment;
                            else
                                offset = static_cast<uint32>(Math::CeilToInt(static_cast<float>(offset) / static_cast<float>(alignment)) * alignment);
                        }
                    }

                    MEMCPY(data + offset, src, typeSize);
                    offset += typeSize;
                }

                // Update buffer.
                m_dataBuffer.CopyInto(data, m_totalAlignedSize);
                m_propertiesDirty = false;
                delete data;
            }

            if (!m_runtimeDirtyTextures.empty())
            {
                m_descriptor.BeginUpdate();

                for (auto& pair : m_runtimeDirtyTextures)
                {
                    auto& txt = m_textures[pair.first];
                    m_descriptor.AddTextureUpdate(txt.binding, pair.second);
                    txt.handle.sid   = pair.second->GetSID();
                    txt.handle.value = pair.second;
                }

                m_descriptor.SendUpdate();

                m_runtimeDirtyTextures.clear();
            }
        }
    }

    void Material::SetTexture(const String& name, Texture* txt)
    {
        uint32 index = 0;
        bool   found = false;
        for (auto& t : m_textures)
        {
            if (t.name.compare(name.c_str()) == 0)
            {
                found = true;
                break;
            }
            index++;
        }

        if (!found)
        {
            LINA_ERR("[Material] -> Can't set the texture because it doesn't exist! {0} {1}", name, m_path.c_str());
            return;
        }

        m_runtimeDirtyTextures[index] = txt;
    }

    void Material::SetTexture(uint32 index, Texture* txt)
    {
        if (m_textures.size() <= index)
        {
            LINA_ERR("[Material] -> Can't set the texture because index is out of bounds! {0} {1}", index, m_path.c_str());
            return;
        }

        m_runtimeDirtyTextures[index] = txt;
    }

    void Material::SaveToFile()
    {
        if (ApplicationInfo::GetAppMode() != ApplicationMode::Editor)
            return;

        if (Utility::FileExists(m_path))
            Utility::DeleteFileInPath(m_path);

        Serialization::SaveToFile<Material>(m_path, *this);
    }

    MaterialPropertyBase* Material::CreateProperty(MaterialPropertyType type, const String& name)
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
            p = new MaterialProperty<Vector4>();
            break;
        }
        case MaterialPropertyType::Mat4: {
            p = new MaterialProperty<Matrix>();
            break;
        }
        default:
            LINA_ASSERT(false, "Type not found!");
        }

        p->m_name = name;
        p->m_type = type;
        return p;
    }
} // namespace Lina::Graphics
