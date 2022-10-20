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

#include "Resource/Shader.hpp"
#include "Core/Backend.hpp"
#include "Utility/Vulkan/SPIRVUtility.hpp"
#include "Core/CommonApplication.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/ResourceManager.hpp"
#include "Utility/ShaderUtility.hpp"

#include <sstream>
#include <iostream>
#include <string>
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Shader::~Shader()
    {
        for (auto& [stage, mod] : m_modules)
        {
            if (mod.ptr != nullptr)
                vkDestroyShaderModule(Backend::Get()->GetDevice(), mod.ptr, Backend::Get()->GetAllocator());
        }
    }

    Resources::Resource* Shader::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        LoadFromArchive(archive);

        if (!CreateShaderModules())
        {
            LINA_ERR("[Shader Loader - File] -> Could not load shader! {0}", m_path);
        }
        else
            GeneratePipeline();

        return this;
    }

    Resources::Resource* Shader::LoadFromFile(const String& path)
    {
        LoadAssetData();

        bool shouldRegen = m_modules.empty();

        if (!shouldRegen)
        {
            for (auto& p : m_modules)
            {
                if (p.second.byteCode.empty())
                {
                    shouldRegen = true;
                    break;
                }
            }
        }

        if (shouldRegen)
        {
            m_modules.clear();

            // Get the text from file.
            std::ifstream file;
            file.open(path.c_str());
            std::stringstream buffer;
            buffer << file.rdbuf();
            m_text = buffer.str().c_str();
            file.close();

            CheckShaderProperties();
            CheckIfModuleExists("Vtx", ShaderStage::Vertex, "#LINA_VS");
            CheckIfModuleExists("Fs", ShaderStage::Fragment, "#LINA_FS");
            CheckIfModuleExists("Geo", ShaderStage::Geometry, "#LINA_GEO");
            CheckIfModuleExists("Tesc", ShaderStage::TesellationControl, "#LINA_TESC");
            CheckIfModuleExists("Tese", ShaderStage::TesellationEval, "#LINA_TESE");
            CheckIfModuleExists("Comp", ShaderStage::Compute, "#LINA_COMP");

            GenerateByteCode();

            if (ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
                SaveAssetData();
        }

        if (!CreateShaderModules())
        {
            LINA_ERR("[Shader Loader - File] -> Could not load shader! {0}", path);
        }
        else
            GeneratePipeline();

        // We do not need the byte code anymore.
        for (auto& [stage, mod] : m_modules)
            mod.byteCode.clear();

        return this;
    }

    void Shader::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        LoadAssetData();

        bool shouldRegen = m_modules.empty();

        if (!shouldRegen)
        {
            for (auto& p : m_modules)
            {
                if (p.second.byteCode.empty())
                {
                    shouldRegen = true;
                    break;
                }
            }
        }

        if (shouldRegen)
        {
            m_modules.clear();

            std::ifstream file;
            file.open(m_path.c_str());
            std::stringstream buffer;
            buffer << file.rdbuf();
            m_text = buffer.str().c_str();
            file.close();

            CheckShaderProperties();
            CheckIfModuleExists("Vtx", ShaderStage::Vertex, "#LINA_VS");
            CheckIfModuleExists("Fs", ShaderStage::Fragment, "#LINA_FS");
            CheckIfModuleExists("Geo", ShaderStage::Geometry, "#LINA_GEO");
            CheckIfModuleExists("Tesc", ShaderStage::TesellationControl, "#LINA_TESC");
            CheckIfModuleExists("Tese", ShaderStage::TesellationEval, "#LINA_TESE");
            CheckIfModuleExists("Comp", ShaderStage::Compute, "#LINA_COMP");
            GenerateByteCode();
        }

        SaveToArchive(archive);
    }

    void Shader::SaveToArchive(Serialization::Archive<OStream>& archive)
    {
        uint8 pipelineTypeInt = static_cast<uint8>(m_pipelineType);
        archive(pipelineTypeInt);

        archive(m_renderPasses);
        archive(m_drawPassMask);

        const uint32 reflectedPropertyCount = static_cast<uint32>(m_reflectedProperties.size());
        archive(reflectedPropertyCount);

        for (auto& p : m_reflectedProperties)
        {
            const uint8 descTypeInt = static_cast<uint8>(p.descriptorType);
            const uint8 propTypeInt = static_cast<uint8>(p.propertyType);
            archive(descTypeInt);
            archive(propTypeInt);
            archive(p.descriptorBinding);
            archive(p.name);
        }

        const uint32 moduleSize = static_cast<uint32>(m_modules.size());
        archive(moduleSize);

        uint32 i = 0;
        for (auto& [stage, mod] : m_modules)
        {
            const uint8 stageInt = static_cast<uint8>(stage);
            archive(stageInt);
            archive(mod.moduleName);
            archive(mod.moduleText);

            const uint32 byteCodeSize = static_cast<uint32>(mod.byteCode.size());
            archive(byteCodeSize);

            if (byteCodeSize > 0)
            {
                uint8* ptr = (uint8*)&mod.byteCode[0];
                archive.GetStream().WriteEndianSafe(ptr, byteCodeSize * sizeof(unsigned int));
            }

            i++;
        }
    }

    void Shader::LoadFromArchive(Serialization::Archive<IStream>& archive)
    {
        uint8 pipelineTypeInt = 0;
        archive(pipelineTypeInt);
        m_pipelineType = static_cast<PipelineType>(pipelineTypeInt);

        archive(m_renderPasses);
        archive(m_drawPassMask);

        uint32 reflectedPropertyCount = 0;
        archive(reflectedPropertyCount);

        for (uint32 i = 0; i < reflectedPropertyCount; i++)
        {
            uint8  descTypeInt       = 0;
            uint8  propTypeInt       = 0;
            uint32 descriptorBinding = 0;
            String name              = "";
            archive(descTypeInt);
            archive(propTypeInt);
            archive(descriptorBinding);
            archive(name);

            ShaderReflectedProperty prop;
            prop.descriptorType    = static_cast<DescriptorType>(descTypeInt);
            prop.propertyType      = static_cast<MaterialPropertyType>(propTypeInt);
            prop.descriptorBinding = descriptorBinding;
            prop.name              = name;
            m_reflectedProperties.push_back(prop);
        }

        uint32 moduleSize = 0;
        archive(moduleSize);

        for (uint32 i = 0; i < moduleSize; i++)
        {
            ShaderModule mod;
            uint8        stageInt = 0;
            archive(stageInt);
            archive(mod.moduleName);
            archive(mod.moduleText);

            uint32 byteCodeSize = 0;
            archive(byteCodeSize);

            if (byteCodeSize > 0)
            {
                mod.byteCode.resize(byteCodeSize);
                archive.GetStream().ReadEndianSafe(&mod.byteCode[0], byteCodeSize * sizeof(unsigned int));
            }

            const ShaderStage stage = static_cast<ShaderStage>(stageInt);
            m_modules[stage]        = mod;
        }
    }

    void Shader::CheckShaderProperties()
    {
        // Add includes first
        ShaderUtility::AddIncludes(m_text);

        // draw passes
        m_drawPassMask = 0;
        m_drawPassMask = ShaderUtility::GetPassMask(m_text);

        // render passes
        ShaderUtility::FillRenderPasses(m_text, m_renderPasses, &m_pipelineType);

        // material props
        m_reflectedProperties.clear();
        ShaderUtility::FillMaterialProperties(m_text, m_reflectedProperties);
    }

    void Shader::CheckIfModuleExists(const String& name, ShaderStage stage, const String& define)
    {
        ShaderModule mod;
        mod.moduleText = ShaderUtility::GetShaderStageText(m_text, define);

        if (mod.moduleText.compare("") != 0)
        {
            mod.moduleName   = name;
            m_modules[stage] = mod;
        }
    }

    void Shader::GenerateByteCode()
    {
        if (!RenderEngine::Get()->IsInitialized())
            return;

        for (auto& [stage, mod] : m_modules)
        {
            mod.byteCode.clear();
            SPIRVUtility::GLSLToSPV(stage, mod.moduleText.c_str(), mod.byteCode);
        }
    }

    bool Shader::CreateShaderModules()
    {
        if (!RenderEngine::Get()->IsInitialized())
        {
            LINA_ERR("[Shader] -> Could not create shader, render engine is not initialized!");
            return false;
        }

        for (auto& [stage, mod] : m_modules)
        {
            VkShaderModuleCreateInfo vtxCreateInfo = {
                .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext    = nullptr,
                .codeSize = mod.byteCode.size() * sizeof(uint32_t),
                .pCode    = mod.byteCode.data(),
            };

            VkResult res = vkCreateShaderModule(Backend::Get()->GetDevice(), &vtxCreateInfo, nullptr, &mod.ptr);
            if (res != VK_SUCCESS)
            {
                LINA_ERR("[Shader] -> Could not create Vertex shader module!");
                return false;
            }
        }

        return true;
    }

    void Shader::GeneratePipeline()
    {
        if (!RenderEngine::Get()->IsInitialized())
            return;

        m_pipelineLayout = PipelineLayout{};
        HashSet<uint32> sets;

        HashMap<uint32, Vector<ShaderDescriptorSetInfo>> setInfos;

        for (auto& [stage, mod] : m_modules)
        {
            Vector<PushConstantRange> pcrs = ShaderUtility::CheckForPushConstants(stage, mod.moduleText);

            for (auto& pcr : pcrs)
                m_pipelineLayout.AddPushConstant(pcr);

            Vector<ShaderDescriptorSetInfo> infos = ShaderUtility::CheckForDescriptorSets(GetShaderStage(stage), mod.moduleText);

            for (auto& i : infos)
                setInfos[i.setIndex].push_back(i);
        }

        // Only care about material set.
        auto&                                       set = setInfos[2];
        HashMap<uint32, DescriptorSetLayoutBinding> bindings;

        for (auto& i : set)
        {
            if (bindings.contains(i.bindingIndex))
                bindings[i.bindingIndex].stageFlags |= i.stageFlags;
            else
            {
                bindings[i.bindingIndex] = DescriptorSetLayoutBinding{
                    .binding         = i.bindingIndex,
                    .descriptorCount = i.descriptorCount,
                    .stageFlags      = i.stageFlags,
                    .type            = i.type,
                };
            }
        }

        for (auto& b : bindings)
            m_materialLayout.AddBinding(b.second);

        m_materialLayout.Create();

        m_pipelineLayout.AddDescriptorSetLayout(RenderEngine::Get()->GetLayout(DescriptorSetType::GlobalSet));
        m_pipelineLayout.AddDescriptorSetLayout(RenderEngine::Get()->GetLayout(DescriptorSetType::PassSet));
        m_pipelineLayout.AddDescriptorSetLayout(m_materialLayout);

        m_pipelineLayout.Create();

        for (auto rpi : m_renderPasses)
        {
            const RenderPassType rp = static_cast<RenderPassType>(rpi);

            ColorBlendAttachmentState blendAttachment = ColorBlendAttachmentState{
                .blendEnable         = true,
                .srcColorBlendFactor = BlendFactor::SrcAlpha,
                .dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
                .colorBlendOp        = BlendOp::Add,
                .srcAlphaBlendFactor = BlendFactor::One,
                .dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha,
                .alphaBlendOp        = BlendOp::Add,
                .componentFlags      = ColorComponentFlags::RGBA,
            };

            m_pipelines[rp] = Pipeline{
                .pipelineType             = m_pipelineType,
                .depthTestEnabled         = true,
                .depthWriteEnabled        = true,
                .depthCompareOp           = CompareOp::LEqual,
                .topology                 = Topology::TriangleList,
                .polygonMode              = PolygonMode::Fill,
                .blendAttachment          = blendAttachment,
                .colorBlendLogicOpEnabled = false,
                .colorBlendLogicOp        = LogicOp::Copy,
            };

            if (m_pipelineType == PipelineType::Default)
            {
                m_pipelines[rp].cullMode  = CullMode::Back;
                m_pipelines[rp].frontFace = FrontFace::ClockWise;
            }
            else if (m_pipelineType == PipelineType::NoVertex)
            {
                m_pipelines[rp].cullMode  = CullMode::Front;
                m_pipelines[rp].frontFace = FrontFace::AntiClockWise;
            }
            else if (m_pipelineType == PipelineType::GUI)
            {
                m_pipelines[rp].cullMode  = CullMode::None;
                m_pipelines[rp].frontFace = FrontFace::ClockWise;
            }

            m_pipelines[rp].SetShader(this).SetLayout(m_pipelineLayout).SetRenderPass(RenderEngine::Get()->GetRenderer().GetRenderPass(rp)).Create();
        }
    }
} // namespace Lina::Graphics
