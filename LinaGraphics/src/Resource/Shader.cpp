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
#include "Core/ResourceDataManager.hpp"
#include "Core/Backend.hpp"
#include "Utility/SPIRVUtility.hpp"

#include <sstream>
#include <iostream>
#include <string>
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void* Shader::LoadFromMemory(const String& path, unsigned char* data, size_t dataSize)
    {
        IResource::SetSID(path);
        LoadAssetData();
        m_text = String(reinterpret_cast<char*>(data), dataSize);

        m_vertexText = GetShaderStageText(m_text, "#LINA_VS");
        m_fragText   = GetShaderStageText(m_text, "#LINA_FS");

        if (m_assetData.geoShader)
            m_geoText = GetShaderStageText(m_text, "#LINA_GEO");

        if (!CreateShaderModules())
            LINA_ERR("[Shader Loader - Memory] -> Could not load shader! {0}", path);

        return static_cast<void*>(this);
    }

    void* Shader::LoadFromFile(const String& path)
    {
        IResource::SetSID(path);
        LoadAssetData();

        // Get the text from file.
        std::ifstream file;
        file.open(path.c_str());
        std::stringstream buffer;
        buffer << file.rdbuf();
        m_text = buffer.str().c_str();
        file.close();

        m_vertexText = GetShaderStageText(m_text, "#LINA_VS");
        m_fragText   = GetShaderStageText(m_text, "#LINA_FS");

        if (m_assetData.geoShader)
            m_geoText = GetShaderStageText(m_text, "#LINA_GEO");

        if (!CreateShaderModules())
            LINA_ERR("[Shader Loader - File] -> Could not load shader! {0}", path);
        return static_cast<void*>(this);
    }

    void Shader::LoadAssetData()
    {
        auto dm = Resources::ResourceDataManager::Get();
        if (!dm->Exists(m_sid))
            SaveAssetData();

        m_assetData.dummy = Resources::ResourceDataManager::Get()->GetValue<int>(m_sid, "Dummy");
        m_assetData.dummy = Resources::ResourceDataManager::Get()->GetValue<bool>(m_sid, "GeoShader");
    }

    void Shader::SaveAssetData()
    {
        Resources::ResourceDataManager::Get()->SetValue<int>(m_sid, "Dummy", 0);
        Resources::ResourceDataManager::Get()->SetValue<bool>(m_sid, "GeoShader", false);
        Resources::ResourceDataManager::Get()->Save();
    }

    String Shader::GetShaderStageText(const String& shader, const String& defineStart)
    {
        std::istringstream f(shader.c_str());
        std::string        line;
        bool               append = false;
        String             out    = "";
        while (std::getline(f, line))
        {
            if (append && line.compare("#LINA_END") == 0)
                break;

            if (append)
            {
                out.append(line.c_str());
                out.append("\n");
            }

            if (line.compare(defineStart.c_str()) == 0)
                append = true;
        }
        return out;
    }

    bool Shader::CreateShaderModules()
    {
        Vector<unsigned int> vertexBuffer;
        Vector<unsigned int> fragBuffer;
        SPIRVUtility::GLSLToSPV(ShaderStage::Vertex, m_vertexText.c_str(), vertexBuffer);
        SPIRVUtility::GLSLToSPV(ShaderStage::Fragment, m_fragText.c_str(), fragBuffer);

        // Vtx shader
        VkShaderModuleCreateInfo vtxCreateInfo = {
            .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext    = nullptr,
            .codeSize = vertexBuffer.size() * sizeof(uint32_t),
            .pCode    = vertexBuffer.data(),
        };

        VkResult res = vkCreateShaderModule(Backend::Get()->GetDevice(), &vtxCreateInfo, nullptr, &_ptrVtx);
        if (res != VK_SUCCESS)
        {
            LINA_ERR("[Shader] -> Could not create Vertex shader module!");
            return false;
        }

        vertexBuffer.clear();

        // Frag shader
        VkShaderModuleCreateInfo fragCreateInfo = {
            .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext    = nullptr,
            .codeSize = fragBuffer.size() * sizeof(uint32_t),
            .pCode    = fragBuffer.data(),
        };

        res = vkCreateShaderModule(Backend::Get()->GetDevice(), &fragCreateInfo, nullptr, &_ptrFrag);

        if (res != VK_SUCCESS)
        {
            LINA_ERR("[Shader] -> Could not create Fragment shader module!");
            return false;
        }

        fragBuffer.clear();

        if (m_assetData.geoShader)
        {
            Vector<unsigned int> geoBuffer(m_geoText.size() / sizeof(uint32_t));
            SPIRVUtility::GLSLToSPV(ShaderStage::Geometry, m_geoText.c_str(), geoBuffer);

            // Vtx shader
            VkShaderModuleCreateInfo geoCreateInfo = {
                .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext    = nullptr,
                .codeSize = geoBuffer.size() * sizeof(uint32_t),
                .pCode    = geoBuffer.data(),
            };

            res = vkCreateShaderModule(Backend::Get()->GetDevice(), &geoCreateInfo, nullptr, &_ptrGeo);
            if (res != VK_SUCCESS)
            {
                LINA_ERR("[Shader] -> Could not create Geometry shader module!");
                return false;
            }

            geoBuffer.clear();
        }

        return true;
    }
} // namespace Lina::Graphics
