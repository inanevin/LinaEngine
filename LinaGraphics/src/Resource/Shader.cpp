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

    Resources::Resource* Shader::LoadFromMemory(const IStream& stream)
    {
       // IResource::SetSID(path);
       // m_text = String(reinterpret_cast<char*>(data), dataSize);

        CheckIfModuleExists("Vtx", ShaderStage::Vertex, "#LINA_VS");
        CheckIfModuleExists("Fs", ShaderStage::Fragment, "#LINA_FS");
        CheckIfModuleExists("Geo", ShaderStage::Geometry, "#LINA_GEO");
        CheckIfModuleExists("Tesc", ShaderStage::TesellationControl, "#LINA_TESC");
        CheckIfModuleExists("Tese", ShaderStage::TesellationEval, "#LINA_TESE");
        CheckIfModuleExists("Comp", ShaderStage::Compute, "#LINA_COMP");

        LoadAssetData();

        bool missing = false;
        for (auto& [stage, mod] : m_modules)
        {
            if (mod.byteCode.empty())
            {
                missing = true;
                break;
            }
        }

        // In standalone build we should already have loaded the byte code via asset data
        // So only re-generate if its missing.
        if (missing)
            GenerateByteCode();

        if (!CreateShaderModules())
        {
            LINA_ERR("[Shader Loader - Memory] -> Could not load shader! {0}", m_path);
        }
        else
            GeneratePipeline();

        // We do not need the byte code anymore.
        for (auto& [stage, mod] : m_modules)
            mod.byteCode.clear();

        return this;
    }

    Resources::Resource* Shader::LoadFromFile(const String& path)
    {
        // IResource::SetSID(path);

        // Get the text from file.
        std::ifstream file;
        file.open(path.c_str());
        std::stringstream buffer;
        buffer << file.rdbuf();
        m_text = buffer.str().c_str();
        file.close();

        CheckIfModuleExists("Vtx", ShaderStage::Vertex, "#LINA_VS");
        CheckIfModuleExists("Fs", ShaderStage::Fragment, "#LINA_FS");
        CheckIfModuleExists("Geo", ShaderStage::Geometry, "#LINA_GEO");
        CheckIfModuleExists("Tesc", ShaderStage::TesellationControl, "#LINA_TESC");
        CheckIfModuleExists("Tese", ShaderStage::TesellationEval, "#LINA_TESE");
        CheckIfModuleExists("Comp", ShaderStage::Compute, "#LINA_COMP");
        LoadAssetData();

        bool missing = false;
        for (auto& [stage, mod] : m_modules)
        {
            if (mod.byteCode.empty())
            {
                missing = true;
                break;
            }
        }

        if (missing)
            GenerateByteCode();

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

    void Shader::LoadAssetData()
    {
        //auto dm = Resources::ResourceDataManager::Get();
        //if (!dm->Exists(m_sid))
        //    SaveAssetData();
        //
        //for (auto& [stage, mod] : m_modules)
        //    mod.byteCode = dm->GetValue<Vector<unsigned int>>(m_sid, mod.moduleName);
    }

    void Shader::SaveAssetData()
    {
       // auto* dm = Resources::ResourceDataManager::Get();
       // 
       // dm->CleanSlate(m_sid);
       // 
       // for (auto& [stage, mod] : m_modules)
       //     dm->SetValue<Vector<unsigned int>>(m_sid, mod.moduleName, mod.byteCode);
       // 
       // dm->Save();
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

        if (g_appInfo.GetAppMode() == ApplicationMode::Editor)
            SaveAssetData();
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

        for (auto& [stage, mod] : m_modules)
        {
            Vector<PushConstantRange> pcrs = ShaderUtility::CheckForPushConstants(stage, mod.moduleText);

            for (auto& pcr : pcrs)
                m_pipelineLayout.AddPushConstant(pcr);

            Vector<ShaderDescriptorSetInfo> infos = ShaderUtility::CheckForDescriptorSets(GetShaderStage(stage), mod.moduleText);

            for (auto& i : infos)
                sets.insert(i.setIndex);
        }

        for (auto setIndex : sets)
        {
            DescriptorSetLayout& l = RenderEngine::Get()->GetLayout(setIndex);
            m_pipelineLayout.AddDescriptorSetLayout(l);
        }

        m_pipelineLayout.Create();

        m_pipeline = Pipeline{
            .depthTestEnabled  = true,
            .depthWriteEnabled = true,
            .depthCompareOp    = CompareOp::LEqual,
            .viewport          = RenderEngine::Get()->GetViewport(),
            .scissor           = RenderEngine::Get()->GetScissor(),
            .topology          = Topology::TriangleList,
            .polygonMode       = PolygonMode::Fill,
            .cullMode          = CullMode::Back,
        };

        m_pipeline.SetShader(this)
            .SetLayout(m_pipelineLayout)
            .SetRenderPass(RenderEngine::Get()->GetLevelRenderer().GetRP())
            .Create();
    }
} // namespace Lina::Graphics
