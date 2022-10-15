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
#include "Core/ResourceManager.hpp"
#include "PipelineObjects/CommandBuffer.hpp"

namespace Lina::Graphics
{
    Material::~Material()
    {
    }

    Resources::Resource* Material::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        Load(archive);
        return this;
    }

    Resources::Resource* Material::LoadFromFile(const String& path)
    {
        Serialization::LoadFromFile<Material>(path, *this);
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
            return;

        FindShader();
    }

    void Material::FindShader()
    {
        auto* manager = Resources::ResourceManager::Get();

        if (manager->Exists<Shader>(m_shader.sid))
            m_shader.value = manager->GetResource<Shader>(m_shader.sid);
    }

    void Material::SetShader(Shader* shader)
    {
        m_shader.sid   = shader->GetSID();
        m_shader.value = shader;
    }

    void Material::BindPipelineAndDescriptors(CommandBuffer& cmd)
    {
        auto& pipeline = m_shader.value->GetPipeline();
        pipeline.Bind(cmd, PipelineBindPoint::Graphics);

       // DescriptorSet& descSet = renderer.GetGlobalSet();
       // DescriptorSet& objSet = renderer.GetObjectSet();
       // DescriptorSet& txtSet = renderer.GetTextureSet();
       //
       // uint32_t uniformOffset = VulkanUtility::PadUniformBufferSize(sizeof(GPUSceneData)) * renderer.GetFrameIndex();
       // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 0, 1, &descSet, 1, &uniformOffset);
       // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 1, 1, &objSet, 0, nullptr);

        // cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, pipeline._layout, 2, 1, &txtSet, 0, nullptr);
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
