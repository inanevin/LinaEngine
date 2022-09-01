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

#include "Core/Level.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/ResourceHandle.hpp"
#include "Core/World.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Serialization/Serialization.hpp"
#include <fstream>
#include <stdio.h>

namespace Lina::World
{
    void Level::Install()
    {
        m_world.Initialize();
        LINA_TRACE("Level installed: {0}", m_path);
    }

    Level::Level(const Level& lvl)
    {
        m_usedResources = lvl.m_usedResources;
    }

    Resources::Resource* Level::LoadFromMemory(const IStream& stream)
    {
        // std::string        dataStr((char*)data, dataSize);
        // std::istringstream stream(dataStr, std::ios::binary);
        // {
        //     cereal::PortableBinaryInputArchive iarchive(stream);
        //     iarchive(*this);
        //     m_world.LoadFromArchive(iarchive);
        // }
        //
        // stream.clear();
        // IResource::SetSID(path);
        return this;
    }

    Resources::Resource* Level::LoadFromFile(const String& path)
    {
        Serialization::LoadFromFile<Level>(path, *this);
        return this;
    }

    void Level::Uninstall()
    {
        m_world.Shutdown();
        LINA_TRACE("Level uninstalled: {0}", m_path);
    }

    void Level::SaveToFile(const String& path)
    {
        m_sid  = HashedString(path.c_str());
        m_path = path;
        m_tid  = GetTypeID<Level>();

        // Find the resources used by this level by adding all currently active resource handles.
        auto*       storage = Resources::ResourceManager::Get();
        const auto& caches  = storage->GetCaches();
        for (const auto& [tid, cache] : caches)
        {
            const auto& handles = cache->GetResourceHandles();
            for (auto handle : handles)
            {
                if (g_defaultResources.IsEngineResource(tid, handle->sid))
                    continue;

                const StringID sid = handle->sid;
                Resource*      res = static_cast<Resource*>(storage->GetResource(tid, sid));
                m_usedResources.push_back(linatl::make_pair(handle->tid, res->GetPath()));
            }
        }

        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        Serialization::SaveToFile<Level>(path, *this);
    }

    void Level::LoadWithoutWorld(const String& path)
    {
    }

    void Level::ResourcesLoaded()
    {
        m_resourcesLoaded.store(true);
    }
} // namespace Lina::World
