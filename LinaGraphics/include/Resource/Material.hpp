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

    private:
        struct TextureProperty
        {
            Resources::ResourceHandle<Texture> handle;
            String                             name    = "";
            uint32                             binding = 0;
        };

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

            for (auto& txt : m_textures)
            {
                archive(txt.binding);
                archive(txt.name);
                txt.handle.Save(archive);
            }
        }

        template <class Archive> void Load(Archive& archive)
        {
            m_shader.Load(archive);

            uint32 propSize = 0;

            archive(propSize);

            m_savedProperties.clear();
            for (uint32 i = 0; i < propSize; i++)
            {
                uint8  typeInt = 0;
                String name    = "";
                archive(typeInt);
                archive(name);

                const MaterialPropertyType type = static_cast<MaterialPropertyType>(typeInt);
                auto                       prop = CreateProperty(type, name);
                prop->LoadFromArchive(archive);
                m_savedProperties.push_back(prop);
            }

            m_totalPropertySize = 0;

            for (auto p : m_properties)
                m_totalPropertySize += p->GetTypeSize();

            m_totalAlignedSize = GetPropertiesTotalAlignedSize();

            uint32 texturesSize = 0;
            archive(texturesSize);

            m_savedTextures.clear();
            for (uint32 i = 0; i < texturesSize; i++)
            {
                TextureProperty prop;
                archive(prop.binding);
                archive(prop.name);
                prop.handle.Load(archive);
                m_savedTextures.push_back(prop);
            }
        }

        virtual Resource* LoadFromMemory(Serialization::Archive<IStream>& archive) override;
        virtual Resource* LoadFromFile(const String& path) override;
        virtual void      WriteToPackage(Serialization::Archive<OStream>& archive) override;
        virtual void      LoadReferences() override;
        virtual void      SaveToFile() override;
        void              SetShader(Shader* shader);
        void              BindPipelineAndDescriptors(CommandBuffer& cmd, RenderPassType rpType);
        void              SetTexture(const String& name, Texture* texture);
        void              SetTexture(uint32 index, Texture* texture);
        uint32            GetPropertyTypeAlignment(MaterialPropertyType type);
        uint32            GetPropertiesTotalAlignedSize();

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
            int32 selected = -1;
            int32 index    = 0;
            for (auto& p : m_properties)
            {
                if (p->GetName().compare(name) == 0)
                {
                    selected = index;
                    break;
                }
                index++;
            }

            if (selected == -1)
            {
                LINA_WARN("[Material] -> Can't set property because name isn't found!");
                return;
            }

            SetProperty(static_cast<uint32>(selected), value);
        }

        inline Resources::ResourceHandle<Shader>& GetShaderHandle()
        {
            return m_shader;
        }

    private:
        friend class RenderEngine;

        void                  SetupProperties();
        void                  CreateBuffer();
        MaterialPropertyBase* CreateProperty(MaterialPropertyType type, const String& name);
        void                  ChangedShader();

    private:
        Resources::ResourceHandle<Shader> m_shader;
        Vector<MaterialPropertyBase*>     m_properties;
        Vector<MaterialPropertyBase*>     m_savedProperties;
        Vector<TextureProperty>           m_textures;
        Vector<TextureProperty>           m_savedTextures;

        HashMap<uint32, Texture*> m_runtimeDirtyTextures;
        uint32                    m_totalPropertySize = 0;
        uint32                    m_totalAlignedSize  = 0;
        DescriptorSet             m_descriptor;
        DescriptorPool            m_descriptorPool;
        Buffer                    m_dataBuffer;
        bool                      m_propertiesDirty = false;
    };

} // namespace Lina::Graphics

#endif
