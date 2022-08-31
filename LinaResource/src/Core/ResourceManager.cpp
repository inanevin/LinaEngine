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

#include "Core/ResourceManager.hpp"
#include "Core/ResourceLoader.hpp"

namespace Lina::Resources
{
    ResourceManager* ResourceManager::s_instance = nullptr;

    void ResourceManager::InjectResourceLoader(ResourceLoader* loader)
    {
        if (m_loader != nullptr)
            delete m_loader;

        m_loader = loader;
    }

    TypeID ResourceManager::GetTypeIDFromExtension(const String& ext)
    {
        for (auto& [tid, cache] : m_caches)
        {
            const ResourceTypeData& typeData = cache->GetTypeData();

            for (const auto& str : typeData.associatedExtensions)
            {
                if (str.compare(ext) == 0)
                    return tid;
            }
        }
        return 0;
    }

    void ResourceManager::Initialize()
    {
        m_loader = new ResourceLoader();
    }

    void ResourceManager::Shutdown()
    {
        delete m_loader;

        for (auto& [tid, cache] : m_caches)
            cache->Shutdown();
    }

    void ResourceManager::LoadEngineResources()
    {
        m_loader->LoadEngineResources();
    }

    void ResourceManager::SaveAllMetadata()
    {
        if (Utility::FileExists("Resources/lina.metadata"))
            Utility::DeleteFileInPath("Resources/lina.metadata");

        Serialization::Archive<OStream> archive;
        archive.GetStream().CreateReserve(100000);

        for (auto [tid, cache] : m_caches)
            cache->SaveMetadataToArchive(archive);

        Serialization::SaveArchiveToFile("Resources/lina.metadata", archive);
    }

    void ResourceManager::LoadAllMetadata()
    {
        if (!Utility::FileExists("Resources/lina.metadata"))
            return;

        Serialization::Archive<IStream> archive = Serialization::LoadArchiveFromFile("Resources/lina.metadata");

        for (auto [tid, cache] : m_caches)
            cache->LoadMetadataFromArchive(archive);

        archive.GetStream().Destroy();
    }
} // namespace Lina::Resources
