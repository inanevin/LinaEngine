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

#ifndef Material_HPP
#define Material_HPP

#include "Core/Resource.hpp"
#include "Core/ResourceHandle.hpp"
#include "Shader.hpp"
#include "PipelineObjects/DescriptorSet.hpp"
#include "PipelineObjects/DescriptorPool.hpp"
#include "PipelineObjects/Buffer.hpp"
#include "Core/ResourceHandle.hpp"
#include "Resource/MaterialProperty.hpp"
#include "Resource/Texture.hpp"
#include "Serialization/StringSerialization.hpp"
#include "Serialization/Serialization.hpp"

namespace Lina::Graphics
{
    class CommandBuffer;
    class MaterialPropertyBase;

    class Material : public Resources::Resource
    {

    public:
        Material() = default;
        virtual ~Material();

        template <class Archive> void Save(Archive& archive)
        {
            m_shader.Save(archive);

            const uint32 propSize = static_cast<uint32>(m_properties.size());

            archive(propSize);

            for (auto& p : m_properties)
            {
                const String name = p->GetName();
                const uint8  type = static_cast<uint8>(p->GetType());
                archive(type);
                archive(name);
                p->SaveToArchive(archive);
            }

            const uint32 texturesSize = static_cast<uint32>(m_textures.size());
            archive(texturesSize);

            for (auto& pair : m_textures)
            {
                archive(pair.first);
                pair.second.Save(archive);
            }
        }

        template <class Archive> void Load(Archive& archive)
        {
            m_shader.Load(archive);

            uint32 propSize = 0;

            archive(propSize);

            for (uint32 i = 0; i < propSize; i++)
            {
                uint8  typeInt = 0;
                String name    = "";
                archive(typeInt);
                archive(name);

                const MaterialPropertyType type = static_cast<MaterialPropertyType>(typeInt);
                auto                       prop = AddProperty(type, name);
                prop->LoadFromArchive(archive);
            }

            m_totalPropertySize = 0;
            for (auto p : m_properties)
                m_totalPropertySize += static_cast<uint8>(p->GetTypeSize());

            uint32 texturesSize = 0;
            archive(texturesSize);

            for (uint32 i = 0; i < texturesSize; i++)
            {
                uint32 index = 0;
                archive(index);
                m_textures[index].Load(archive);
            }
        }

        virtual Resource* LoadFromMemory(Serialization::Archive<IStream>& archive) override;
        virtual Resource* LoadFromFile(const String& path) override;
        virtual void      WriteToPackage(Serialization::Archive<OStream>& archive) override;
        virtual void      LoadReferences() override;
        virtual void      SaveToFile() override;
        void              SetShader(Shader* shader);
        void              BindPipelineAndDescriptors(CommandBuffer& cmd, RenderPassType rpType);
        void              SetTexture(uint32 binding, Texture* texture);

        template <typename T> void SetProperty(uint32 index, T value)
        {
            if (m_properties.size() <= index)
            {
                LINA_WARN("[Material] -> Can't set property because index is overflowing.");
                return;
            }

            MaterialProperty<T>* p = static_cast<MaterialProperty<T>*>(m_properties[index]);
            p->SetValue(value);
            m_propertiesDirty = true;
        }

        template <typename T> void SetProperty(const String& name, T value)
        {
            int32 index = -1;
            for (auto& p : m_properties)
            {
                if (p->GetName().compare(name) == 0)
                {
                    break;
                }
                index++;
            }

            if (index == -1)
            {
                LINA_WARN("[Material] -> Can't set property because name isn't found!");
                return;
            }

            SetProperty(static_cast<uint32>(index), value);
        }

        inline Resources::ResourceHandle<Shader>& GetShaderHandle()
        {
            return m_shader;
        }

    private:
        friend class RenderEngine;

        void                  SetupProperties();
        void                  CreateBuffer();
        MaterialPropertyBase* AddProperty(MaterialPropertyType type, const String& name);
        void                  ChangedShader();

    private:
        Resources::ResourceHandle<Shader>                   m_shader;
        Vector<MaterialPropertyBase*>                       m_properties;
        HashMap<uint32, Resources::ResourceHandle<Texture>> m_textures;
        HashMap<uint32, Texture*>                           m_runtimeDirtyTextures;
        uint32                                              m_totalPropertySize = 0;
        DescriptorSet                                       m_descriptor;
        DescriptorPool                                      m_descriptorPool;
        Buffer                                              m_dataBuffer;
        bool                                                m_propertiesDirty = false;
    };

} // namespace Lina::Graphics

#endif
