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

#include "Core/ResourceLoader.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/ResourceCache.hpp"
#include "Core/Resource.hpp"
#include "Data/Streams.hpp"
#include "Core/CommonEngine.hpp"
#include "JobSystem/JobSystem.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace Lina::Resources
{
    void ResourceLoader::LoadLevelResources(const Vector<Pair<TypeID, String>>& resources)
    {
        auto* rm = Resources::ResourceManager::Get();

        // Remove the currently loaded resources if they are not used by the next level.
        const auto& caches = rm->GetCaches();

        const TypeID levelTid = g_levelTypeID;
        for (auto [tid, cache] : caches)
        {
            if (tid != levelTid)
                cache->UnloadUnusedLevelResources(resources);
        }

        // Add the ones that are not currently loaded.
        HashMap<PackageType, Vector<Pair<TypeID, String>>> toLoad;
        for (auto& pair : resources)
        {
            const StringID sid = TO_SID(pair.second);
            if (!rm->Exists(pair.first, sid))
            {
                const PackageType pt = rm->GetCache(pair.first)->GetTypeData().packageType;
                toLoad[pt].push_back(linatl::make_pair(pair.first, pair.second));
            }
        }

        int totalFiles = 0;

        for (auto& pair : toLoad)
            totalFiles += static_cast<int>(pair.second.size());

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{.title = "Loading level resources", .totalFiles = totalFiles});

        const double time = Time::GetCPUTime();

        for (auto& pair : toLoad)
            LoadResources(pair.first, pair.second, true);

        const double diff = Time::GetCPUTime() - time;
        LINA_TRACE("[Resource Loader] -> Loading level resources took {0} seconds.", diff);

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>(Event::EResourceProgressEnded{});

        ResourceManager::Get()->LoadReferences();
    }

    void ResourceLoader::LoadEngineResources()
    {
        Vector<Pair<TypeID, String>> engineResources;
        auto&                        engineRes  = g_defaultResources.GetEngineResources();
        int                          totalFiles = 0;
        for (auto& [tid, vec] : engineRes)
            totalFiles += static_cast<int>(vec.size());

        const double time = Time::GetCPUTime();
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{.title = "Loading engine resources", .totalFiles = totalFiles});

        for (auto& pair : engineRes)
        {
            for (auto& res : pair.second)
                engineResources.push_back(linatl::make_pair(pair.first, res));
        }

        LoadResources(PackageType::Static, engineResources, true);

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>(Event::EResourceProgressEnded{});
        const double diff = Time::GetCPUTime() - time;
        LINA_TRACE("[Resource Loader] -> Loading engine resources took {0} seconds.", diff);

        ResourceManager::Get()->LoadReferences();
    }

    void ResourceLoader::LoadResource(TypeID tid, const String& path, bool async)
    {
        const ResourceTypeData& td                = ResourceManager::Get()->GetCache(tid)->GetTypeData();
        const String            packageName       = GetPackageTypeName(td.packageType);
        const String            packagePath       = "Packages/" + packageName + ".linapckg";
        const uint32            totalResourceSize = 1;

        Serialization::Archive<IStream> archive = Serialization::LoadArchiveFromFile(packagePath);

        TypeID         resTid    = 0;
        StringID       resSid    = 0;
        uint32         resSize   = 0;
        const StringID targetSid = TO_SID(path);

        while (!archive.GetStream().IsCompleted())
        {
            archive(resTid);
            archive(resSid);
            archive(resSize);

            if (tid == resTid && resSid == targetSid)
            {
                LoadResource(tid, path, archive, resSize, async);
                break;
            }
        }

        if (async)
            JobSystem::Get()->GetResourceExecutor().Wait();

        archive.GetStream().Destroy();
    }

    void ResourceLoader::LoadResources(PackageType packageType, const Vector<Pair<TypeID, String>>& resources, bool async)
    {
        const String packageName       = GetPackageTypeName(packageType);
        const String packagePath       = "Packages/" + packageName + ".linapckg";
        const uint32 totalResourceSize = static_cast<uint32>(resources.size());
        uint32       loadedResources   = 0;

        Serialization::Archive<IStream> archive = Serialization::LoadArchiveFromFile(packagePath);

        TypeID   tid  = 0;
        StringID sid  = 0;
        uint32   size = 0;

        while (loadedResources < totalResourceSize && !archive.GetStream().IsCompleted())
        {
            archive(tid);
            archive(sid);
            archive(size);

            for (auto& pair : resources)
            {
                const StringID resSid = TO_SID(pair.second);
                if (tid == pair.first && sid == resSid)
                {
                    LoadResource(tid, pair.second, archive, size, async);
                    archive.GetStream().SkipBy(size);
                    loadedResources++;
                }
            }
        }

        if (async)
            JobSystem::Get()->GetResourceExecutor().Wait();

        archive.GetStream().Destroy();
    }

    void ResourceLoader::LoadResource(TypeID tid, const String& path, Serialization::Archive<IStream>& archive, uint32 size, bool async)
    {
        const StringID sid = HashedString(path.c_str()).value();
        auto*          rm  = ResourceManager::Get();

        if (rm->Exists(tid, sid))
            return;

        Serialization::Archive<IStream> copyArchive;
        copyArchive.GetStream().Create(archive.GetStream().GetDataCurrent(), size);
        copyArchive.GetStream().Seek(0);

        const auto& load = [rm, tid, sid, path, size, copyArchive, this]() {
            auto*     cache = rm->GetCache(tid);
            Resource* res   = cache->Create(sid);
            res->m_sid      = sid;
            res->m_path     = path;
            res->m_tid      = tid;

            Serialization::Archive<IStream> finalArchive = copyArchive;
            res->LoadFromMemory(finalArchive);
            finalArchive.GetStream().Destroy();

            LOCK_GUARD(m_mtx);
            Event::EventSystem::Get()->Trigger<Event::EResourceLoaded>(Event::EResourceLoaded{.tid = tid, .sid = sid});
            Event::EventSystem::Get()->Trigger<Event::EResourceProgressUpdated>(Event::EResourceProgressUpdated{.currentResource = path});
        };

        if (async)
            JobSystem::Get()->GetResourceExecutor().SilentAsync(load);
        else
            load();
    }

} // namespace Lina::Resources
