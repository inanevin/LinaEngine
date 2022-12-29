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

#include "Graphics/Resource/Font.hpp"
#include "Core/CommonEngine.hpp"
#include "Serialization/VectorSerialization.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Log/Log.hpp"

#define LINAVG_TEXT_SUPPORT
#include "LinaVG/LinaVG.hpp"

#include <fstream>

namespace Lina::Graphics
{
    Resources::Resource* Font::LoadFromMemory(Serialization::Archive<IStream>& archive)
    {
        LoadAssetData();

        return this;
    }

    Resources::Resource* Font::LoadFromFile(const char* path)
    {
        LoadAssetData();
        return this;
    }

    void Font::WriteToPackage(Serialization::Archive<OStream>& archive)
    {
        LoadAssetData();

        if (m_assetData.file.empty())
        {
            std::ifstream file(m_path.c_str());
            if (!file)
            {
                LINA_ERR("[Font] -> Could not open font for writing to package! {0}", m_path.c_str());
                return;
            }

            // Size
            file.seekg(0, std::ios::end);
            std::streampos length = file.tellg();
            file.seekg(0, std::ios::beg);

            // Into vec
            m_assetData.file = Vector<char>(length);
            file.read(&m_assetData.file[0], length);
        }

        // Save
        SaveToArchive(archive);
    }

    void Font::GenerateFontImpl(bool isSDF, int size)
    {
        m_assetData.isSDF = isSDF;
        m_assetData.size  = size;

        const int contentScale = static_cast<int>(RenderEngine::Get()->GetScreen().GetContentScale().x);

        if (ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
            m_handles[size] = LinaVG::LoadFont(m_path.c_str(), m_assetData.isSDF, m_assetData.size * contentScale);
        else
        {
            uint8* ptr      = (uint8*)(m_assetData.file.data());
            m_handles[size] = LinaVG::LoadFontFromMemory(ptr, m_assetData.file.size(), m_assetData.isSDF, m_assetData.size * contentScale);
        }
    }

    void Font::ClearBuffers()
    {
        if (ApplicationInfo::GetAppMode() != ApplicationMode::Editor)
            m_assetData.file.clear();
    }

    uint32 Font::GetHandle()
    {
        return m_handles.begin()->second;
    }

    uint32 Font::GetHandle(int size)
    {
        return m_handles[size];
    }

    void Font::SaveToArchive(Serialization::Archive<OStream>& archive)
    {
        archive(m_assetData.isSDF);
        archive(m_assetData.size);
        archive(m_assetData.file);
    }

    void Font::LoadFromArchive(Serialization::Archive<IStream>& archive)
    {
        archive(m_assetData.isSDF);
        archive(m_assetData.size);
        archive(m_assetData.file);
    }

} // namespace Lina::Graphics
