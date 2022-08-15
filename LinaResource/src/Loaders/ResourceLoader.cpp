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

#include "Loaders/ResourceLoader.hpp"
#include "Core/ResourceStorage.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "EventSystem/EventSystem.hpp"

namespace Lina::Resources
{
    bool ResourceLoader::LoadSingleResourceFromMemory(const String& path, Vector<unsigned char>& data)
    {
        auto*        storage   = Resources::ResourceStorage::Get();
        const String extension = Utility::GetFileExtension(Utility::GetFileNameOnly(path));
        TypeID       tid       = storage->GetTypeIDFromExtension(extension);
        StringID     sid       = HashedString(path.c_str()).value();

        m_lastResourceTypeID = tid;

        if (!storage->Exists(tid, sid))
        {
            auto&      typeData       = storage->GetTypeData(tid);
            IResource* res            = typeData.createFunc();
            void*      loadedResource = res->LoadFromMemory(path, &data[0], data.size());
            if (loadedResource == nullptr)
                return false;

            storage->Add(loadedResource, tid, sid);
            Event::EventSystem::Get()->Trigger<Event::EResourceLoaded>(Event::EResourceLoaded{tid, sid});
            data.clear();
        }

        return true;
    }

    bool ResourceLoader::LoadSingleResourceFromFile(TypeID tid, const String& path)
    {
        auto*    storage = ResourceStorage::Get();
        StringID sid     = HashedString(path.c_str()).value();

        if (!storage->Exists(tid, sid))
        {
            auto&      typeData       = storage->GetTypeData(tid);
            IResource* res            = typeData.createFunc();
            void*      loadedResource = res->LoadFromFile(path);

            if (loadedResource == nullptr)
                return false;

            storage->Add(loadedResource, tid, sid);

            Event::EventSystem::Get()->Trigger<Event::EResourceLoaded>(Event::EResourceLoaded{tid, sid});
        }

        return true;
    }

    void ResourceLoader::UnloadUnusedResources(const HashMap<TypeID, HashSet<String>>& levelResources)
    {
        ResourceStorage* storage = ResourceStorage::Get();

        // Find all resources that are not used by the next level, unload them.
        auto& cacheMap = storage->m_caches;

        // For every resource type
        for (auto& [tid, cache] : cacheMap)
        {
            const auto& typeData = cache.GetTypeData();
            if (typeData.packageType == PackageType::Static || typeData.packageType == PackageType::Level)
                continue;

            const auto&            engineRes = g_defaultResources.GetEngineResources();
            const HashSet<String>& set       = levelResources.at(tid);

            Vector<StringID> toRemove;
            toRemove.reserve(cache.m_resources.size() / 2);

            for (auto [sid, ptr] : cache.m_resources)
            {
                const auto& it = linatl::find_if(set.begin(), set.end(), [sid](const String& str) {
                    return HashedString(str.c_str()).value() == sid;
                });

                // If we're going to remove this resource, make sure it's not an engine resource.
                if (it == set.end() && !g_defaultResources.IsEngineResource(tid, sid))
                    toRemove.push_back(sid);
            }

            for (StringID sid : toRemove)
                cache.Unload(sid);
        }
    }

} // namespace Lina::Resources