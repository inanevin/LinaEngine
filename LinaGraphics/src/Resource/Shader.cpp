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
#include <sstream>
#include <iostream>
#include <string>

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
        LINA_TRACE("{0}", out.c_str());
        return out;
    }
} // namespace Lina::Graphics
