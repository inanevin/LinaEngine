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
#include "PipelineObjects/Buffer.hpp"
#include "Core/ResourceHandle.hpp"
#include "Resource/MaterialProperty.hpp"
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
            archive(m_isInstanced);

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
        }

        template <class Archive> void Load(Archive& archive)
        {
            m_shader.Load(archive);
            archive(m_isInstanced);

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
        }

        virtual Resource* LoadFromMemory(Serialization::Archive<IStream>& archive) override;
        virtual Resource* LoadFromFile(const String& path) override;
        virtual void      WriteToPackage(Serialization::Archive<OStream>& archive) override;
        virtual void      LoadReferences() override;
        virtual void      SaveToFile() override;
        void              SetShader(Shader* shader);
        void              BindPipelineAndDescriptors(CommandBuffer& cmd, RenderPassType rpType);

        inline Resources::ResourceHandle<Shader>& GetShaderHandle()
        {
            return m_shader;
        }

        inline bool GetIsInstanced()
        {
            return m_isInstanced;
        }

    private:
        void                  FindShader();
        void                  SetupProperties();
        void                  CheckDescriptorAndBuffer();
        MaterialPropertyBase* AddProperty(MaterialPropertyType type, const String& name);

    private:
        bool                              m_isInstanced = false;
        Resources::ResourceHandle<Shader> m_shader;
        Vector<MaterialPropertyBase*>     m_properties;
        uint32                            m_totalPropertySize = 0;
        DescriptorSet                     m_descriptor;
        Buffer                            m_dataBuffer;
    };

} // namespace Lina::Graphics

#endif
